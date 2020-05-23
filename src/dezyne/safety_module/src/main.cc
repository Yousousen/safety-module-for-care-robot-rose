// TODO: Clean up the entire code. Order it logically.
// Safety module
// The module contains a pointer to a care robot, which provides the module
// with a representation of a care robot that is useful to it.  In the file,
// retrieve functions update data from a sensor.
#include <cstdlib>
#include <ctime>
#include <iomanip>
#include <string>
#include <thread>
#include <chrono>
#include <functional>
#include <map>
#include <fstream>

#include <stdint.h>
#include <fcntl.h>
#include <linux/fb.h>
#include <sys/mman.h>
#include <sys/ioctl.h>
#include <time.h>
#include <poll.h>
#include <dirent.h>
#include <string.h>
#include <unistd.h>
#include <semaphore.h>
#include <rtdm/ipc.h>
#include <signal.h>
#include <pthread.h>
#include <errno.h>
#include <linux/input.h>
#include <linux/fb.h>
#include <dzn/runtime.hh>
#include <dzn/locator.hh>
#include "RTIMULib.h"

// Own
#include "System.hh"
#include "constants.hh"
#include "calculus.hh"
#include "physics.hh"
#include "care_robot.hh"

/*
 * Maximum allowed values. Exceeding these values is unsafe behavior of the
 * robot. TODO: Obtain this from file or command line.
 */
#define MAX_KE 300
#define MAX_RE 300
#define MAX_FORCE 50
#define MAX_FORCE_PAYLOAD 80

/**** Prototypes ****/
// Run the safety module
ErrorCode_t roll();

/*** LED matrix functions ***/
// Construct and destruct the frame buffer
int is_framebuffer_device(const struct dirent *dir);
int open_fbdev(const char *dev_name);
void initialise_framebuffer();
void destruct_framebuffer();
// Light the led matrix in the specified color.
void light_led(unsigned color);
// Reset the led matrix to low.
void reset_led();

double retrieve_acceleration();
double retrieve_angular_displacement();
double retrieve_force();
double retrieve_position();

/*** data set functions ***/
void set_acceleration(double acceleration);
void set_angular_displacement(double angular_displacement);
void set_arm_force(double arm_force, bool has_payload);
void set_arm_position(double arm_force, bool is_moving);

bool robot_is_moving();
bool arm_is_folded();
bool arm_has_payload();

/*** sampling functions ***/
void sample_acceleration(double* f, const int nsamples);
void sample_angular_displacement(double* f, const int nsamples);

/*** retrievers and resolvers ***/
void retrieve_ke_from_acc();
Behavior::type resolve_ke_from_acc();
void retrieve_re_from_ang_vel();
Behavior::type resolve_re_from_ang_vel();
void retrieve_arm_force();
Behavior::type resolve_arm_force();
void retrieve_arm_pos();
Behavior::type resolve_arm_pos();

void initialise();
void destruct();

/* void what_triggered(bool acc, bool angacc, bool force, bool pos); */

/*** Threads related ***/

/** Real-time threads **/
// Sends colors to nrt_light_led.
static void* rt_light_led(void* arg);

// Sets the acceleration, retrieves acceleration from
// nrt_retrieve_acceleration.
static void* rt_retrieve_acceleration(void* arg);

// Sets the angular displacement, retrieves angular displacement from
// nrt_retrieve_angular_displacement.
static void* rt_retrieve_angular_displacement(void* arg);

// Sets the arm force, retrieves arm force from
// nrt_retrieve_arm_force.
static void* rt_retrieve_arm_force(void* arg);

// Sets the arm position, retrieves arm position from
// nrt_retrieve_arm_position.
static void* rt_retrieve_arm_position(void* arg);

/* Thread to sample acceleration continuously.
 * Currently does the following (TODO: probably split):
 * - set number of samples
 * - samples acceleration
 * - ∫ a dt
 * - calculates kinetic energy
 */
static void* rt_sample_acceleration(void* arg);

/*
 * Thread to sample angular velocity continuously.
 * Currently does the following (TODO: probably split):
 * - set number of samples
 * - samples angular velocity
 * - calculates rotational energy
 */
static void* rt_sample_angular_velocity(void* arg);

// Thread to execute checks periodically.
static void rt_checks(void* arg);

// Starts a periodic real-time thread.
static void *rt_periodic_thread_body(void *arg);


/** Non-real-time threads **/
// The LED matrix driver.
// blocks on read to read a color from the XDDP socket, indefinitely.
static void* nrt_light_led(void *arg);

/* The acceleration and angular velocity driver. Sends current acceleration and
 * current angular velocity over an XDDP socket to rt_retrieve_acceleration and
 * rt_retrieve_angular_displacement.
 */
static void* nrt_retrieve_imu(void *arg);

/* The arm force driver. Sends current force over an XDDP socket to
 * rt_retrieve_arm_force
 */
static void* nrt_retrieve_arm_force(void *arg);

/* The arm position driver. Sends current position over an XDDP socket to
 * rt_retrieve_arm_position
 */
static void* nrt_retrieve_arm_position(void *arg);

// Starts a periodic non-real-time thread.
static void *nrt_periodic_thread_body(void *arg);


// Start a periodic timer with an offset.
// the function returns a new periodic task object, with the period set to t.
struct periodic_task *start_periodic_timer(unsigned long long offset_in_us,
        int period);

// Wait for next activation of the periodic thread.
void wait_next_activation(struct periodic_task *ptask);

static void fail(const char *reason);

void dzn_light_led(char* color);

static int initialise_mutexes(std::map<std::string, pthread_mutex_t>& m);
static int destruct_mutexes(std::map<std::string, pthread_mutex_t>& m);

static int initialise_semaphores(std::map<std::string, sem_t>& s);
static int destruct_semaphores(std::map<std::string, sem_t>& s);

// calls function f in a thread safe manner, locking the mutex before the call.
static inline int safe_call(std::function<void()> f, pthread_mutex_t* mutex) {
    if (0 != (errno = pthread_mutex_lock(mutex))) { // Lock
        perror("pthread_mutex_lock failed");
        return NOT_OK;
    }

    f();

    if (0 != (errno = pthread_mutex_unlock(mutex))) { // Unlock
        perror("pthread_mutex_unlock failed");
        return NOT_OK;
    }
}

// Add t microseconds to timespec ts.
static inline void timespec_add_us(struct timespec *ts, unsigned long long t)
{
    // (1µs) = 1000 * (1ns)
    t *= 1000;
    // Add number of nanoseconds already in ts to t.
    t += ts->tv_nsec;
    // To set tv_sec to the correct number we look at how many seconds of
    // nanoseconds there are in period.
    while (t >= NSEC_PER_SEC) {
        t -= NSEC_PER_SEC;
	ts->tv_sec += 1;
    }
    // The nanoseconds left.
    ts->tv_nsec = t;
}

// Convert g-force to m/s^2
static double inline gforce_to_si(double gforce) {
    return gforce * 9.81;
}

void write_to_fd(int fd, double what) {
    int ret;
    char buf[BUFSIZE];
    snprintf(buf, BUFSIZE, "%f", what);
    ret = write(fd, buf, BUFSIZE);
    if (ret <= 0)
        fail("write");
}


/*** Global variables ***/
struct Position* position = nullptr;
struct Velocity* velocity = nullptr;
struct Acceleration* acceleration = nullptr;
struct AngularDisplacement* angular_displacement = nullptr;
struct AngularAcceleration* angular_acceleration = nullptr;

double kinetic_energy;
double rotational_energy;
double arm_force;
int arm_position;

CareRobotRose* rose = nullptr;

// For LED Matrix 
struct fb_t *fb;
int fbfd = 0;

// For sensors
RTIMU *imu;
int imu_poll_interval;

static char color[SIZE];

// Semaphores
std::map<std::string, sem_t> semaphore;
// Mutexes
std::map<std::string, pthread_mutex_t> mutex;

auto main() -> int {
    initialise();
    int r = roll();
    if (r != OK) return EXIT_FAILURE;
    destruct();
}


static int initialise_semaphores(std::map<std::string, sem_t>& s) {
    // Make semaphores
    s["led"];
    s["retrieve_acc"];
    s["retrieve_ang_disp"];
    s["retrieve_arm_force"];
    s["retrieve_arm_pos"];
    s["sample_acc"];
    s["sample_ang_vel"];

    // Initialise semaphores.
    for (auto it = s.begin(); it != s.end(); ++it) {
        if (0 != (errno = sem_init(&it->second, 0, 0))) {
            perror("sem_init() failed");
            return NOT_OK;
        }
    }
    return OK;
}

static int destruct_semaphores(std::map<std::string, sem_t>& s) {
    for (auto it = s.begin(); it != s.end(); ++it) {
        if (0 != (errno = sem_destroy(&it->second))) {
            perror("pthread_mutex_destroy() failed");
            return NOT_OK;
        }
    }
    return OK;
}

static int initialise_mutexes(std::map<std::string, pthread_mutex_t>& m) {
    // Make mutex
    m["color"];
    m["fb"];
    m["acc"];
    m["ang_disp"];
    m["arm_force"];
    m["arm_pos"];
    m["ke"];
    m["re"];

    // Initialise mutexes.
    for (auto it = m.begin(); it != m.end(); ++it) {
        if (0 != (errno = pthread_mutex_init(&it->second, NULL))) {
            perror("pthread_mutex_init() failed");
            return NOT_OK;
        }
    }
    return OK;
}

static int destruct_mutexes(std::map<std::string, pthread_mutex_t>& m) {
    for (auto it = m.begin(); it != m.end(); ++it) {
        if (0 != (errno = pthread_mutex_destroy(&it->second))) {
            perror("pthread_mutex_destroy() failed");
            return NOT_OK;
        }
    }
    return OK;
}
ErrorCode_t roll() {
    // Initialise dezyne locator and runtime.
    IResolver iResolver({});

    // Bind resolvers
    iResolver.in.resolve_ke_from_acc = []() -> Behavior::type {
        int r;
        Behavior::type type;
        double ke;

        r = safe_call([&ke]() { ke = kinetic_energy; }, &mutex["ke"]);
        if (r != OK) exit(EXIT_FAILURE);

        if (ke > MAX_KE)
            type = Behavior::type::Unsafe;
        else
            type = Behavior::type::Safe;
        return type;
    };

    iResolver.in.resolve_re_from_ang_vel = []() -> Behavior::type {
        int r;
        Behavior::type type;
        double re;

        r = safe_call([&re]() { re = rotational_energy; }, &mutex["re"]);
        if (r != OK) exit(EXIT_FAILURE);

        if (re > MAX_RE)
            type =  Behavior::type::Unsafe;
        else
            type =  Behavior::type::Safe;
        return type;
    };

    iResolver.in.resolve_arm_force = []() -> Behavior::type {
        return Behavior::type::Safe;
    };

    iResolver.in.resolve_arm_pos = []() -> Behavior::type {
        return Behavior::type::Safe;
    };

    /* iResolver.in.resolve_arm_force = []() -> Behavior::type { */
    /*     int r; */
    /*     Behavior::type type; */
    /*     double str; */
    /*     bool has_payload; */

    /*     r = safe_call([&]() { str = arm_force; has_payload = */
    /*             arm_has_payload(); }, &mutex["arm_force"]); */
    /*     if (r != OK) exit(EXIT_FAILURE); */

    /*     if ( has_payload && str > MAX_FORCE_PAYLOAD) */
    /*         type = Behavior::type::Unsafe; */
    /*     else if (str > MAX_FORCE) */
    /*         type = Behavior::type::Unsafe; */
    /*     else */
    /*         type = Behavior::type::Safe; */
    /*     return type; */
    /* }; */

    /* iResolver.in.resolve_arm_pos = []() -> Behavior::type { */
    /*     int r; */
    /*     Behavior::type type; */
    /*     bool folded; */
    /*     bool is_moving; */

    /*     r = safe_call([&]() { folded = arm_is_folded(); is_moving = */
    /*             robot_is_moving(); }, &mutex["arm_pos"]); */
    /*     if (r != OK) exit(EXIT_FAILURE); */

    /*     if (is_moving && !folded) */
    /*         type = Behavior::type::Unsafe; */
    /*     else */
    /*         type = Behavior::type::Safe; */
    /*     return type; */
    /* }; */

    dzn::locator locator;
    dzn::runtime runtime;
    locator.set(runtime);
    locator.set(iResolver);
    auto output = std::ofstream("dzn_output.log");
    locator.set(static_cast<std::ostream&>(output));

    System s(locator);
    s.dzn_meta.name = "System";


    /*
     * Bind dezyne functions with C++ functions.
     */
    /* s.iController.out.what_triggered = what_triggered; */
    s.iLEDControl.in.initialise_framebuffer = initialise_framebuffer;
    s.iLEDControl.in.destruct_framebuffer = destruct_framebuffer;
    s.iLEDControl.in.light_led_red = dzn_light_led;
    s.iLEDControl.in.light_led_blue = dzn_light_led;
    s.iLEDControl.in.reset_led = reset_led;
    s.iAccelerationSensor.in.retrieve_ke_from_acc = retrieve_ke_from_acc;
    s.iAngularVelocitySensor.in.retrieve_re_from_ang_vel =
        retrieve_re_from_ang_vel;
    s.iArmPositionSensor.in.retrieve_arm_pos = retrieve_arm_pos;
    s.iArmForceSensor.in.retrieve_arm_force = retrieve_arm_force;

    // Check bindings
    s.check_bindings();

    /*
     * Initialise system
     */
    s.iController.in.initialise();

    // Initialse inertial measurement unit related matters
    RTIMUSettings *settings = new RTIMUSettings("./RTIMULib");
    imu = RTIMU::createIMU(settings);
    if ((imu == NULL) || (imu->IMUType() == RTIMU_TYPE_NULL)) {
        printf("No IMU found\n");
        exit(1);
    }

    //  This is an opportunity to manually override any settings before the
    //  call IMUInit

    //  Set up IMU
    imu->IMUInit();

    //  This is a convenient place to change fusion parameters.
    imu->setSlerpPower(0.02);
    imu->setGyroEnable(true);
    imu->setAccelEnable(true);
    imu->setCompassEnable(true);

    // Set poll interval.
    imu_poll_interval = imu->IMUGetPollInterval() * 1E5;

    /*** Threads related ***/
    struct sched_param rtparam = { .sched_priority = 42 };
    pthread_attr_t rtattr, nrtattr;
    sigset_t set;
    int sig;

    // real-time thread that starts safety checks periodically.
    static pthread_t th_rt_checks;
    // real-time thread for lighting the LED matrix.
    static pthread_t th_rt_light_led;
    // real-time threads for retrieving data.
    static pthread_t th_rt_ret_acc, th_rt_ret_ang_disp;
    static pthread_t th_rt_ret_arm_force, th_rt_ret_arm_pos;
    // real-time threads for sampling sensor data.
    static pthread_t th_rt_sample_acc, th_rt_sample_ang_disp;

    // non-real-time thread for lighting the LED matrix.
    static pthread_t th_nrt_light_led;
    // non-real-time thread for retrieving sensor data from the IMU and ROS.
    static pthread_t th_nrt_ret_imu, th_nrt_ret_arm_force, th_nrt_ret_arm_pos;

    // Information about periodic  threads.
    static struct th_info th_info;

    // Arguments to threads.
    struct threadargs threadargs;

    sigemptyset(&set);
    sigaddset(&set, SIGINT);
    sigaddset(&set, SIGTERM);
    sigaddset(&set, SIGHUP);

    pthread_attr_init(&nrtattr);
    pthread_attr_setdetachstate(&nrtattr, PTHREAD_CREATE_JOINABLE);
    pthread_attr_setinheritsched(&nrtattr, PTHREAD_EXPLICIT_SCHED);
    pthread_attr_setschedpolicy(&nrtattr, SCHED_OTHER);

    pthread_attr_init(&rtattr);
    pthread_attr_setdetachstate(&rtattr, PTHREAD_CREATE_JOINABLE);
    pthread_attr_setinheritsched(&rtattr, PTHREAD_EXPLICIT_SCHED);
    pthread_attr_setschedpolicy(&rtattr, SCHED_FIFO);
    pthread_attr_setschedparam(&rtattr, &rtparam);

    // Initialise mutexes
    initialise_mutexes(mutex);
    // Initialise semaphores
    initialise_semaphores(semaphore);

    /*** Start threads ***/
    // Start thread rt_light_led
    errno = pthread_create(&th_rt_light_led, &rtattr, &rt_light_led, NULL);
    if (errno)
        fail("pthread_create");

    // Start thread rt_retrieve_acceleration
    errno = pthread_create(&th_rt_ret_acc, &rtattr, &rt_retrieve_acceleration,
            &threadargs);
    if (errno)
        fail("pthread_create");

    // Start thread rt_retrieve_angular_displacement
    errno = pthread_create(&th_rt_ret_ang_disp, &rtattr,
            &rt_retrieve_angular_displacement, &threadargs);
    if (errno)
        fail("pthread_create");

    // Start thread rt_retrieve_arm_force
    errno = pthread_create(&th_rt_ret_arm_force, &rtattr,
            &rt_retrieve_arm_force, &threadargs);
    if (errno)
        fail("pthread_create");

    // Start thread rt_retrieve_arm_position
    errno = pthread_create(&th_rt_ret_arm_pos, &rtattr,
            &rt_retrieve_arm_position, &threadargs);
    if (errno)
        fail("pthread_create");

    // Start thread rt_sample_acceleration
    errno = pthread_create(&th_rt_sample_acc, &rtattr, &rt_sample_acceleration,
            &threadargs);
    if (errno)
        fail("pthread_create");

    // Start thread rt_sample_angular_velocity
    errno = pthread_create(&th_rt_sample_ang_disp, &rtattr,
            &rt_sample_angular_velocity, &threadargs);
    if (errno)
        fail("pthread_create");

    // Start thread nrt_light_led
    errno = pthread_create(&th_nrt_light_led, &nrtattr, &nrt_light_led, (void*)
            &threadargs);
    if (errno)
        fail("pthread_create");

    // Start thread nrt_retrieve_imu
    errno = pthread_create(&th_nrt_ret_imu, &nrtattr,
            &nrt_retrieve_imu, NULL);
    if (errno)
        fail("pthread_create");

    // Start thread nrt_retrieve_arm_force
    errno = pthread_create(&th_nrt_ret_arm_force, &nrtattr,
            &nrt_retrieve_arm_force, NULL);
    if (errno)
        fail("pthread_create");

    // Start thread nrt_retrieve_arm_position
    errno = pthread_create(&th_nrt_ret_arm_pos, &nrtattr,
            &nrt_retrieve_arm_position, NULL);
    if (errno)
        fail("pthread_create");



    printf("Started running indefinitely.\n");
    std::string input;
#if PERIODIC_CHECKS
    printf("press: q to quit, r to reset\n");

    th_info.body = rt_checks;
    /* th_info.period = 1E6*4;   // 4s */
    /* th_info.period = 1E6/4;      // 250ms */
    th_info.period = imu_poll_interval;      // poll interval based period.
    th_info.s = &s;
    // Start periodic real-time threads.
    errno = pthread_create(&th_rt_checks, &rtattr, &rt_periodic_thread_body,
            &th_info);
    if (errno)
        fail("pthread_create");

    // The safety module runs so long we are in this loop. The LED light can be
    // reset by inputting 'r'.  We quit the loop correctly by inputting 'q'.
    while (1) {
        std::cin >> input;
        /* input = "a"; */
        if (input == "q") {
            break;
        } else if (input == "r") {
            s.iController.in.reset();
        } else if (input == "i") {
            // Purposely here to show illegal exception handler.
            s.iController.in.initialise();
        } else {
            printf("Did not understand input.\n");
        }
    }
#else
    while (1) {
        printf("press: q to quit, d to execute all checks, r to reset\n");
        printf("a to check acc, aa to check ang acc, s to check str, p to check pos\n\n> ");

        // Notify nrt_retrieve_acceleration that it can retrieve acceleration.
        /* sem_post(&semaphore["retrieve_acc"]]); */
        // Notify rt_sample_acceleration that it can go sample acceleration.
        /* sem_post(&sem_sample_acc); */

        std::cin >> input;
        /* input = "a"; */
        if (input == "q") {
            break;
        } else if (input == "d") {
            s.iController.in.do_checks();
        } else if (input == "r") {
            s.iController.in.reset();
        } else if (input == "i") {
            // Purposely here to show illegal exception handler.
            s.iController.in.initialise();
        } else {
            printf("Did not understand input.\n");
        }
    }
#endif
    printf("Stopping\n");

    // Destruct framebuffer
    s.iController.in.destruct();

    // Kill threads
    pthread_cancel(th_nrt_light_led);
    pthread_cancel(th_nrt_ret_arm_force);
    pthread_cancel(th_nrt_ret_arm_pos);
    pthread_cancel(th_nrt_ret_imu);
    pthread_cancel(th_rt_light_led);
    pthread_cancel(th_rt_ret_acc);
    pthread_cancel(th_rt_ret_ang_disp);
    pthread_cancel(th_rt_ret_arm_force);
    pthread_cancel(th_rt_ret_arm_pos);
    pthread_cancel(th_rt_sample_acc);
    pthread_cancel(th_rt_sample_ang_disp);

#if PERIODIC_CHECKS
    pthread_cancel(th_rt_checks);
#endif

    pthread_join(th_nrt_light_led, NULL);
    pthread_join(th_nrt_ret_arm_pos, NULL);
    pthread_join(th_nrt_ret_arm_force, NULL);
    pthread_join(th_nrt_ret_imu, NULL);
    pthread_join(th_rt_light_led, NULL);
    pthread_join(th_rt_ret_acc, NULL);
    pthread_join(th_rt_ret_ang_disp, NULL);
    pthread_join(th_rt_ret_arm_force, NULL);
    pthread_join(th_rt_ret_arm_pos, NULL);
    pthread_join(th_rt_sample_acc, NULL);
    pthread_join(th_rt_sample_ang_disp, NULL);

#if PERIODIC_CHECKS
    pthread_join(th_rt_checks, NULL);
#endif

    // Destroy mutexes
    destruct_mutexes(mutex);

    // Destroy semaphores
    destruct_semaphores(semaphore);

    return OK;
}


void initialise() {
    // Create a rose representation
    rose = new CareRobotRose();

    if (CSV_HAS_POSITION) {
        // Load from CSV.
    } else {
        position = new Position(0,0,0,0);
    }

    if (CSV_HAS_VELOCITY) {
        // Load from CSV.
    } else {
        velocity = new Velocity(0,0,0,0);
    }

    if (CSV_HAS_ACCELERATION) {
        // Load from CSV.
    } else {
        acceleration = new Acceleration(0,0,0,0);
    }

    if (CSV_HAS_ANGULAR_ACCELERATION) {
        // Load from CSV.
    } else {
        angular_acceleration = new AngularAcceleration(0,0,0,0);
    }

    if (CSV_HAS_ANGULAR_VELOCITY) {
        // Load from CSV.
    } else {
        angular_displacement = new AngularDisplacement(0,0,0,0);
    }

    if (CSV_HAS_ANGULAR_ACCELERATION) {
        // Load from CSV.
    } else {
        angular_acceleration = new AngularAcceleration(0,0,0,0);
    }

}

void destruct() {
    if (rose != nullptr)                 delete rose;
    if (position != nullptr)             delete position;
    if (velocity != nullptr)             delete velocity;
    if (acceleration != nullptr)         delete acceleration;
    if (angular_displacement != nullptr)     delete angular_displacement;
    if (angular_acceleration != nullptr) delete angular_acceleration;
}

void light_led(unsigned color) {
    for (int i = 0; i < 8; ++i) {
        for (int j = 0; j < 8; ++j) {
            fb->pixel[i][j]= color;
        }
    }
}

void reset_led() {
    int r;
    r = safe_call([]() { memset(fb, 0, 128); }, &mutex["fb"]);
    if (r != OK) exit(EXIT_FAILURE);
}

void initialise_framebuffer() {
    ErrorCode_t ret = OK;

    // Initialise frame buffer for led matrix.
    printf("Initialising framebuffer...\n");
    fbfd = open_fbdev("RPi-Sense FB");
    if (fbfd <= 0) {
        ret = NOT_OK;
        printf("Error: cannot open framebuffer device.\n");
        return;
    }
    fb = (struct fb_t*) mmap(0, 128, PROT_READ | PROT_WRITE, MAP_SHARED, fbfd, 0);
    if (!fb) {
        ret = NOT_OK;
        printf("Failed to mmap.\n");
        return;
    }
    memset(fb, 0, 128);
    printf("Framebuffer initialised.\n");
}

void destruct_framebuffer() {
    memset(fb, 0, 128);
    munmap(fb, 128);
    close(fbfd);
}


// Check if device is framebuffer.
int is_framebuffer_device(const struct dirent *dir)
{
    return strncmp(FB_DEV_NAME, dir->d_name,
            strlen(FB_DEV_NAME)-1) == 0;
}

// Open framebuffer device.
int open_fbdev(const char *dev_name) {
    struct dirent **namelist;
    int i, ndev;
    int fd = -1;
    struct fb_fix_screeninfo fix_info;

    ndev = scandir(DEV_FB, &namelist, is_framebuffer_device, versionsort);
    if (ndev <= 0)
        return ndev;

    for (i = 0; i < ndev; i++) {
        char fname[64];

        snprintf(fname, sizeof(fname),
                "%s/%s", DEV_FB, namelist[i]->d_name);
        fd = open(fname, O_RDWR);
        if (fd < 0)
            continue;
        ioctl(fd, FBIOGET_FSCREENINFO, &fix_info);
        if (strcmp(dev_name, fix_info.id) == 0)
            break;
        close(fd);
        fd = -1;
    }
    for (i = 0; i < ndev; i++)
        free(namelist[i]);

    return fd;
}


double retrieve_acceleration() {
    srand((unsigned)time(NULL));
    return rand() % 5 + 1;
}

double retrieve_angular_displacement() {
    srand((unsigned)time(NULL));
    return rand() % 5 + 1;
}

double retrieve_force() {
    srand((unsigned)time(NULL));
    return rand() % MAX_FORCE_PAYLOAD;
}

double retrieve_position() {
    srand((unsigned)time(NULL));
    return rand() % 10;
}

void set_acceleration(double acceleration) {
    auto previous = ::acceleration->current;
    ::acceleration->current = acceleration;
    ::acceleration->change = ::acceleration->current - previous;
}

void set_angular_displacement(double angular_displacement) {
    auto previous = ::angular_displacement->current;
    ::angular_displacement->current = angular_displacement;
    ::angular_displacement->change = ::angular_displacement->current - previous;
}

void set_arm_force(double arm_force, bool has_payload) {
    auto previous = rose->arm->current_force;
    rose->arm->current_force = arm_force;
    rose->arm->change_force = arm_force - previous;
    rose->arm->has_payload = has_payload;
}

void set_arm_position(double arm_position, bool is_moving) {
    auto previous = rose->arm->current_position;
    rose->arm->current_position = arm_position;
    rose->arm->change_position = arm_position - previous;
    rose->is_moving = is_moving;
}


void sample_acceleration(double* f, const int nsamples) {
    printf("# rectangles: ");
    int r;
    int i;
    for (i = 0; i < nsamples; ++i) {
        double curr;

        r = safe_call([&curr]() { curr = acceleration->current; }, &mutex["acc"]);
        if (r != OK) exit(EXIT_FAILURE);
        f[i] = curr;

        /* std::this_thread::sleep_for(std::chrono::microseconds(CHANGE_IN_TIME_MICRO)); */
    }
    printf("%d, ", i);
}


void sample_angular_displacement(double* f, const int nsamples) {
    printf("# rectangles: ");
    int i;
    for (i = 0; i < nsamples; ++i) {
        retrieve_angular_displacement();
        f[i] = angular_displacement->current;
        /* std::this_thread::sleep_for(std::chrono::milliseconds(CHANGE_IN_TIME_MS)); */
        std::this_thread::sleep_for(std::chrono::microseconds(CHANGE_IN_TIME_MICRO));
    }
    printf("%d, ", i);
}


void retrieve_ke_from_acc() {
    // Currently rt_sample_acceleration sets kinetic energy, so we need not do
    // anything here.
}

void retrieve_re_from_ang_vel() {
    // Currently rt_sample_angular_velocity sets rotational energy, so we
    // need not do anything here.
}

void retrieve_arm_force() {
    int r = safe_call( []() {
            arm_force = rose->arm->current_force;
            /* printf("arm force = %g\n", arm_force); */
            },
            &mutex["arm_force"]);
    if (r != OK) exit(EXIT_FAILURE);
}

void retrieve_arm_pos() {
    int r = safe_call( []() {
            arm_position = rose->arm->current_position;
            /* printf("arm position = %d\n", arm_position); */
            },
            &mutex["arm_pos"]);
    if (r != OK) exit(EXIT_FAILURE);
}

bool robot_is_moving() {
    return (velocity->current == 0 || rose->velocity->current == 0) ? false : true;
}

bool arm_is_folded() {
    return (arm_position == 0 ? true : false);
}

bool arm_has_payload() {
    return rose->arm->has_payload;
}

/* void what_triggered(bool acc, bool angacc, bool str, bool pos) { */
/*     if (!(acc || angacc || str || pos)) return; */
/*     printf(">>> what triggered:\n\n"); */
/*     printf("Unsafe behavior caused by:\n"); */
/*     if (acc)    printf("acceleration\n"); */
/*     if (angacc) printf("angular acceleration\n"); */
/*     if (str)    printf("arm force\n"); */
/*     if (pos)    printf("arm position\n"); */
/*     printf("<<<\n\n"); */
/* } */

static void* rt_light_led(void* arg) {
    int n = 0;
    int len;
    int ret;
    int r;
    int s;
    char buf[128];
    struct sockaddr_ipc saddr;
    size_t poolsz;

    struct threadargs *args = (struct threadargs *) arg;
    // Initialise XDDP
    /*
     * Get a datagram socket to bind to the RT endpoint. Each
     * endpoint is represented by a port number within the XDDP
     * protocol namespace.
     */
    s = socket(AF_RTIPC, SOCK_DGRAM, IPCPROTO_XDDP);
    if (s < 0) {
        perror("socket");
        exit(EXIT_FAILURE);
    }

    /*
     * Set a local 16k pool for the RT endpoint. Memory needed to
     * convey datagrams will be pulled from this pool, instead of
     * Xenomai's system pool.
     */
    poolsz = 16384; /* bytes */
    ret = setsockopt(s, SOL_XDDP, XDDP_POOLSZ, &poolsz, sizeof(poolsz));
    if (ret)
        fail("setsockopt");
    /*
     * Bind the socket to the port, to setup a proxy to channel
     * traffic to/from the Linux domain.
     *
     * saddr.sipc_port specifies the port number to use.
     */
    memset(&saddr, 0, sizeof(saddr));
    saddr.sipc_family = AF_RTIPC;
    saddr.sipc_port = XDDP_PORT_LIGHT_LED;
    ret = bind(s, (struct sockaddr *)&saddr, sizeof(saddr));
    if (ret)
        fail("bind");

    /*
     * Send a datagram to the NRT endpoint via the proxy.
     * We may pass a NULL destination address, since a
     * bound socket is assigned a default destination
     * address matching the binding address (unless
     * connect(2) was issued before bind(2), in which case
     * the former would prevail).
     */
    while (1) {
        // Wait for an up to send a color to the LED matrix.
        sem_wait(&semaphore["led"]); 
        char msg[SIZE];

        // Get color
        r = safe_call([&msg]() { strcpy(msg, color); }, &mutex["color"]);
        if (r != OK) exit(EXIT_FAILURE);

        len = strlen(msg);
        ret = sendto(s, msg, len, 0, NULL, 0);
        if (ret != len)
            fail("sendto");
        /* printf("%s: sent %d bytes, \"%.*s\"\n", __FUNCTION__, ret, ret, msg); */
    }
    return NULL;
}

static void* nrt_light_led(void *arg) {
    int r;
    struct threadargs *args = (struct threadargs *)arg;

    char buf[128], *devname;
    int fd, ret;
    if (asprintf(&devname, "/dev/rtp%d", XDDP_PORT_LIGHT_LED) < 0)
        fail("asprintf");
    fd = open(devname, O_RDWR);
    free(devname);
    if (fd < 0)
        fail("open");

    while (1) {
        /* Get the next message from rt_light_led. */
        /* read what to color the led buffer in */
        ret = read(fd, buf, sizeof(buf));
        if (ret <= 0)
            fail("read");

        // Convert hex string to int.
        int color = (int)strtol(buf, NULL, 16);

        r = safe_call([=]() { light_led(color); }, &mutex["color"]);
        if (r != OK) exit(EXIT_FAILURE);
    }

    return NULL;
}

static void* rt_retrieve_acceleration(void* arg) {
    int n = 0;
    int len;
    int ret;
    int r;
    int s;
    char buf[BUFSIZE];
    struct sockaddr_ipc saddr;
    size_t poolsz;

    struct threadargs *args = (struct threadargs *)arg;

    // Initialise XDDP
    /*
     * Get a datagram socket to bind to the RT endpoint. Each
     * endpoint is represented by a port number within the XDDP
     * protocol namespace.
     */
    s = socket(AF_RTIPC, SOCK_DGRAM, IPCPROTO_XDDP);
    if (s < 0) {
        perror("socket");
        exit(EXIT_FAILURE);
    }

    /*
     * Set a local 16k pool for the RT endpoint. Memory needed to
     * convey datagrams will be pulled from this pool, instead of
     * Xenomai's system pool.
     */
    poolsz = 16384; /* bytes */
    ret = setsockopt(s, SOL_XDDP, XDDP_POOLSZ, &poolsz, sizeof(poolsz));
    if (ret)
        fail("setsockopt");
    /*
     * Bind the socket to the port, to setup a proxy to channel
     * traffic to/from the Linux domain.
     *
     * saddr.sipc_port specifies the port number to use.
     */
    memset(&saddr, 0, sizeof(saddr));
    saddr.sipc_family = AF_RTIPC;
    saddr.sipc_port = XDDP_PORT_RET_ACC;
    ret = bind(s, (struct sockaddr *)&saddr, sizeof(saddr));
    if (ret)
        fail("bind");

    /*
     * Retrieve a datagrams from the NRT endpoint via the proxy.
     */
    while (1) {
        /* Read packets echoed by the non-real-time thread */
        /*
         * This call blocks if there is no data to receive. This however is not
         * a problem as sample acceleration is not depended on this thread
         * blocking or not. That is, sample acceleration can do its work
         * independent of this thread.
         */
        ret = recvfrom(s, buf, sizeof(buf), 0, NULL, 0);
        if (ret <= 0)
            fail("recvfrom");
        /* printf("   => \"%.*s\" received by peer\n", ret, buf); */
        n = (n + 1) % (sizeof(buf) / sizeof(buf[0]));

        // Set acceleration
        r = safe_call([&buf]() { set_acceleration(atof(buf)); }, &mutex["acc"]);
        if (r != OK) exit(EXIT_FAILURE);
    }
    return NULL;
}

static void* rt_retrieve_angular_displacement(void* arg) {
    int n = 0;
    int len;
    int ret;
    int r;
    int s;
    char buf[BUFSIZE];
    struct sockaddr_ipc saddr;
    size_t poolsz;

    struct threadargs *args = (struct threadargs *)arg;

    // Initialise XDDP
    /*
     * Get a datagram socket to bind to the RT endpoint. Each
     * endpoint is represented by a port number within the XDDP
     * protocol namespace.
     */
    s = socket(AF_RTIPC, SOCK_DGRAM, IPCPROTO_XDDP);
    if (s < 0) {
        perror("socket");
        exit(EXIT_FAILURE);
    }

    /*
     * Set a local 16k pool for the RT endpoint. Memory needed to
     * convey datagrams will be pulled from this pool, instead of
     * Xenomai's system pool.
     */
    poolsz = 16384; /* bytes */
    ret = setsockopt(s, SOL_XDDP, XDDP_POOLSZ, &poolsz, sizeof(poolsz));
    if (ret)
        fail("setsockopt");
    /*
     * Bind the socket to the port, to setup a proxy to channel
     * traffic to/from the Linux domain.
     *
     * saddr.sipc_port specifies the port number to use.
     */
    memset(&saddr, 0, sizeof(saddr));
    saddr.sipc_family = AF_RTIPC;
    saddr.sipc_port = XDDP_PORT_RET_ANG_DISP;
    ret = bind(s, (struct sockaddr *)&saddr, sizeof(saddr));
    if (ret)
        fail("bind");

    /*
     * Retrieve a datagrams from the NRT endpoint via the proxy.
     */
    while (1) {
        // Read packets echoed by the non-real-time thread.
        /*
         * This call blocks if there is no data to receive. This however is not
         * a problem as sample angular velocity is not depended on this thread
         * blocking or not. That is, sample angular velocity can do its work
         * independent of this thread.
         */
        ret = recvfrom(s, buf, sizeof(buf), 0, NULL, 0);
        if (ret <= 0)
            fail("recvfrom");
        /* printf("   => \"%.*s\" received by peer\n", ret, buf); */
        n = (n + 1) % (sizeof(buf) / sizeof(buf[0]));

        // Set angular displacement
        r = safe_call([&buf]() { set_angular_displacement(atof(buf)); },
                &mutex["ang_disp"]);
        if (r != OK) exit(EXIT_FAILURE);
    }
    return NULL;
}

static void* rt_retrieve_arm_force(void* arg) {
    int n = 0;
    int len;
    int ret;
    int r;
    int s;
    char buf[BUFSIZE];
    struct sockaddr_ipc saddr;
    size_t poolsz;

    struct threadargs *args = (struct threadargs *)arg;

    // Initialise XDDP
    /*
     * Get a datagram socket to bind to the RT endpoint. Each
     * endpoint is represented by a port number within the XDDP
     * protocol namespace.
     */
    s = socket(AF_RTIPC, SOCK_DGRAM, IPCPROTO_XDDP);
    if (s < 0) {
        perror("socket");
        exit(EXIT_FAILURE);
    }

    /*
     * Set a local 16k pool for the RT endpoint. Memory needed to
     * convey datagrams will be pulled from this pool, instead of
     * Xenomai's system pool.
     */
    poolsz = 16384; /* bytes */
    ret = setsockopt(s, SOL_XDDP, XDDP_POOLSZ, &poolsz, sizeof(poolsz));
    if (ret)
        fail("setsockopt");
    /*
     * Bind the socket to the port, to setup a proxy to channel
     * traffic to/from the Linux domain.
     *
     * saddr.sipc_port specifies the port number to use.
     */
    memset(&saddr, 0, sizeof(saddr));
    saddr.sipc_family = AF_RTIPC;
    saddr.sipc_port = XDDP_PORT_RET_STR;
    ret = bind(s, (struct sockaddr *)&saddr, sizeof(saddr));
    if (ret)
        fail("bind");

    /*
     * Retrieve a datagrams from the NRT endpoint via the proxy.
     */
    while (1) {
        /* Read packets echoed by the non-real-time thread */
        /*
         * This call blocks if there is no data to receive. This however is not
         * a problem as sample acceleration is not depended on this thread
         * blocking or not. That is, sample acceleration can do its work
         * independent of this thread.
         */
        ret = recvfrom(s, buf, sizeof(buf), 0, NULL, 0);
        if (ret <= 0)
            fail("recvfrom");
        /* printf("   => \"%.*s\" received by peer\n", ret, buf); */
        n = (n + 1) % (sizeof(buf) / sizeof(buf[0]));

        // buf contains both the force and if we have a payload or not.
        double force = atof(buf);
        // has_payload is sent as an integer after the ':' sign.
        bool has_payload = static_cast<bool>((atoi((strchr(buf, ':')+1))));

        // Set arm force
        r = safe_call([=]() { set_arm_force(force, has_payload); },
                &mutex["arm_force"]);
        if (r != OK) exit(EXIT_FAILURE);
    }
    return NULL;
}

static void* rt_retrieve_arm_position(void* arg) {
    int n = 0;
    int len;
    int ret;
    int r;
    int s;
    char buf[BUFSIZE];
    struct sockaddr_ipc saddr;
    size_t poolsz;

    struct threadargs *args = (struct threadargs *)arg;

    // Initialise XDDP
    /*
     * Get a datagram socket to bind to the RT endpoint. Each
     * endpoint is represented by a port number within the XDDP
     * protocol namespace.
     */
    s = socket(AF_RTIPC, SOCK_DGRAM, IPCPROTO_XDDP);
    if (s < 0) {
        perror("socket");
        exit(EXIT_FAILURE);
    }

    /*
     * Set a local 16k pool for the RT endpoint. Memory needed to
     * convey datagrams will be pulled from this pool, instead of
     * Xenomai's system pool.
     */
    poolsz = 16384; /* bytes */
    ret = setsockopt(s, SOL_XDDP, XDDP_POOLSZ, &poolsz, sizeof(poolsz));
    if (ret)
        fail("setsockopt");
    /*
     * Bind the socket to the port, to setup a proxy to channel
     * traffic to/from the Linux domain.
     *
     * saddr.sipc_port specifies the port number to use.
     */
    memset(&saddr, 0, sizeof(saddr));
    saddr.sipc_family = AF_RTIPC;
    saddr.sipc_port = XDDP_PORT_RET_POS;
    ret = bind(s, (struct sockaddr *)&saddr, sizeof(saddr));
    if (ret)
        fail("bind");

    /*
     * Retrieve a datagrams from the NRT endpoint via the proxy.
     */
    while (1) {
        /* Read packets echoed by the non-real-time thread */
        /*
         * This call blocks if there is no data to receive. This however is not
         * a problem as sample acceleration is not depended on this thread
         * blocking or not. That is, sample acceleration can do its work
         * independent of this thread.
         */
        ret = recvfrom(s, buf, sizeof(buf), 0, NULL, 0);
        if (ret <= 0)
            fail("recvfrom");
        /* printf("   => \"%.*s\" received by peer\n", ret, buf); */
        n = (n + 1) % (sizeof(buf) / sizeof(buf[0]));

        // buf contains both the position and if we are moving or not.
        double position = atof(buf);
        // is_moving is sent as an integer after the ':' sign.
        bool is_moving = static_cast<bool>((atoi((strchr(buf, ':')+1))));

        // Set arm position
        r = safe_call([=]() { set_arm_position(position, is_moving); },
                &mutex["arm_pos"]);
        if (r != OK) exit(EXIT_FAILURE);
    }
    return NULL;
}


static void* rt_sample_acceleration(void* arg) {
    int r;
    // numbers of seconds to sample.
    const double nseconds = 0.1;
    // numbers of samples.
    const int nsamples = (int) (nseconds / CHANGE_IN_TIME);

    // Save acceleration in here.
    double a[nsamples];

    while (1) {
        /* sem_wait(&sem_sample_acc); */
        for (int i = 0; i < nsamples; ++i) {
            double curr;
            // Get acceleration
            r = safe_call([&curr]() { curr = acceleration->current; },
                    &mutex["acc"]);
            if (r != OK) exit(EXIT_FAILURE);
            a[i] = curr;

            std::this_thread::sleep_for(std::chrono::microseconds(CHANGE_IN_TIME_MICRO));
        }

        // Numerically integrate to determine change in velocity from
        // acceleration.
        double change_in_velocity = Calculus::trapezoidal_integral(a, nsamples);

        // Calculate kinetic energy from Δv.
        r = safe_call( [=]() {
                    // KE = (1/2)mv^2
                    kinetic_energy = 0.5 * INERTIAL_MASS * change_in_velocity *
                    change_in_velocity;
                    printf("ke = %g\n", kinetic_energy);
                    },
                    &mutex["ke"]);
        if (r != OK) exit(EXIT_FAILURE);
    }
}

static void* rt_sample_angular_velocity(void* arg) {
    int r;
    // Δt (μs)
    const int change_in_time = imu_poll_interval;
    while (1) {
        /* sem_wait(&semaphore["sample_ang_vel"]); */

        double ang_disp1, ang_disp2;

        // Get Θ
        r = safe_call([&ang_disp1]() { ang_disp1 = angular_displacement->current; },
                &mutex["ang_disp"]);
        if (r != OK) exit(EXIT_FAILURE);

        // Wait for change_in_time microseconds.
        std::this_thread::sleep_for(std::chrono::microseconds(change_in_time));

        // Do it again.
        r = safe_call([&ang_disp2]() { ang_disp2 = angular_displacement->current; },
                &mutex["ang_disp"]);
        if (r != OK) exit(EXIT_FAILURE);


        // Calculate ΔΘ
        double change_in_ang_disp = ang_disp2 - ang_disp1;
        const double change_in_time_s = change_in_time * 1E-6;
        double ang_vel = change_in_ang_disp / change_in_time_s;
        r = safe_call( [=]() {
                    // RE = (1/2)Iω^2
                    rotational_energy = 0.5 * MOMENT_OF_INERTIA *
                    ang_vel * ang_vel;
                    printf("re = %g\n", rotational_energy);
                    fflush(stdout);
                    },
                    &mutex["re"]);
        if (r != OK) exit(EXIT_FAILURE);
    }
}

static void* nrt_retrieve_imu(void *arg) {
    struct threadargs *args = (struct threadargs *)arg;

    char *devname;
    int fd_acc, fd_ang_disp, ret;

    // file descriptor for acceleration.
    if (asprintf(&devname, "/dev/rtp%d", XDDP_PORT_RET_ACC) < 0)
        fail("asprintf");
    fd_acc = open(devname, O_RDWR);
    free(devname);
    if (fd_acc < 0)
        fail("open");

    // file descriptor for angular velocity.
    if (asprintf(&devname, "/dev/rtp%d", XDDP_PORT_RET_ANG_DISP) < 0)
        fail("asprintf");
    fd_ang_disp = open(devname, O_RDWR);
    free(devname);
    if (fd_ang_disp < 0)
        fail("open");

    while (1) {
        double acc_largest, accx, accy, accz;
        double ang_disp_largest, ang_dispx, ang_dispy, ang_dispz;
        // Wait for an up before retrieving acceleration from the sense hat
        // driver.
        /* sem_wait(&semaphore["retrieve_acc"]); */

        // Poll at the rate recommended by the IMU.
        usleep(imu_poll_interval);

        while (imu->IMURead()) {
            RTIMU_DATA imuData = imu->getIMUData();
            accx = gforce_to_si(imuData.accel.x());
            accy = gforce_to_si(imuData.accel.y());
            accz = gforce_to_si(imuData.accel.z()-1); // -1 subtracts gravity.
            // angular velocity is in radians.
            ang_dispx = imuData.fusionPose.x();
            ang_dispy = imuData.fusionPose.y();
            ang_dispz = imuData.fusionPose.z();
        }

        /*
         * For both acceleration ang angular velocity, determine which axis had
         * the largest increase. The largest increase will be used to
         * respectively calculate kinetic energy and rotational energy.
         */
        accx = fabs(accx);
        accy = fabs(accy);
        accz = fabs(accz);
        ang_dispx = fabs(ang_dispx);
        ang_dispy = fabs(ang_dispy);
        ang_dispz = fabs(ang_dispz);

        if (accx > accy && accx > accz)
            acc_largest = accx;
        else if (accy > accx && accy > accz)
            acc_largest = accy;
        else
            acc_largest = accz;

        if (ang_dispx > ang_dispy && ang_dispx > ang_dispz)
            ang_disp_largest = ang_dispx;
        else if (ang_dispy > ang_dispx && ang_dispy > ang_dispz)
            ang_disp_largest = ang_dispy;
        else
            ang_disp_largest = ang_dispz;

        // Write retrieved acceleration to rt_retrieve_acceleration.
        write_to_fd(fd_acc, acc_largest);
        // Write retrieved angular velocity to rt_retrieve_angular_displacement.
        write_to_fd(fd_ang_disp, ang_disp_largest);
    }

    return NULL;
}

static void* nrt_retrieve_arm_force(void *arg) {
    struct threadargs *args = (struct threadargs *)arg;
    double force;
    static int has_payload = 0;
    char *devname;
    int fd, ret;
    char buf[BUFSIZE];

    if (asprintf(&devname, "/dev/rtp%d", XDDP_PORT_RET_STR) < 0)
        fail("asprintf");
    fd = open(devname, O_RDWR);
    free(devname);
    if (fd < 0)
        fail("open");

    while (1) {
        // Wait for an up before retrieving arm force.
        // driver.
        /* sem_wait(&semaphore["retrieve_arm_force"]); */

        // Poll at the same rate as the imu.
        usleep(imu_poll_interval);

        /*
         * Here we should retrieve from ROS.
         * But for now, retrieve a random value.
         */
        force = retrieve_force();
        has_payload ^= 1;

        // Write retrieved force to rt_retrieve_force.
        snprintf(buf, BUFSIZE, "%f:%d", force, has_payload);
        ret = write(fd, buf, BUFSIZE);
        if (ret <= 0)
            fail("write");
    }

    return NULL;
}

static void* nrt_retrieve_arm_position(void *arg) {
    struct threadargs *args = (struct threadargs *)arg;
    int position;
    static int has_payload = 0, is_moving = 0;
    char *devname;
    int fd, ret;
    char buf[BUFSIZE];

    if (asprintf(&devname, "/dev/rtp%d", XDDP_PORT_RET_POS) < 0)
        fail("asprintf");
    fd = open(devname, O_RDWR);
    free(devname);
    if (fd < 0)
        fail("open");

    while (1) {
        // Wait for an up before retrieving arm position.
        // driver.
        /* sem_wait(&semaphore["retrieve_arm_force"]); */

        // Poll at the same rate as the imu.
        usleep(imu_poll_interval);

        /*
         * Here we should retrieve from ROS.
         * But for now, retrieve a random value.
         */
        position = retrieve_position();
        if (position < 8) position = 0; else position = 1;
        is_moving ^= 1;

        // Write retrieved position to rt_retrieve_position.
        snprintf(buf, BUFSIZE, "%d:%d", position, is_moving);
        ret = write(fd, buf, BUFSIZE);
        if (ret <= 0)
            fail("write");
    }

    return NULL;
}


static void rt_checks(void* arg) {
    struct threadargs *args = (struct threadargs *)arg;

    // Notify nrt_retrieve_acceleration that it can retrieve acceleration.
    /* sem_post(&semaphore["retrieve_acc"]); */
    // Notify nrt_retrieve_imu that it can retrieve acceleration.
    /* sem_post(&semaphore["retrieve_ang_disp"]); */
    // Notify rt_sample_acceleration that it can go sample acceleration.
    /* sem_post(&sem_sample_acc); */
    // Notify rt_sample_angular_velocity that it can go sample angular
    // velocity.
    /* sem_post(&semaphore["sample_ang_vel"]); */

    /*
     * Since do_checks is implemented in Dezyne, if we want to disable one
     * check we have to remove the check from the linked list in System.dzn.
     * This is a consequence of making the system more generic. 
     */
    (args->s)->iController.in.do_checks();
}

static void *rt_periodic_thread_body(void *arg) {
    struct periodic_task *ptask;
    struct th_info* the_thread = (struct th_info*) arg;
    struct threadargs threadargs;

    // Copy over dezyne system pointer
    threadargs.s = the_thread->s;

    /* ptask = start_periodic_timer(2000000, the_thread->period); */
    ptask = start_periodic_timer(0, the_thread->period);
    if (ptask == NULL) {
        printf("Start Periodic Timer");

        return NULL;
    }

    while(1) {
        wait_next_activation(ptask);
        the_thread->body((void*) &threadargs);
    }

    return NULL;
}

static void *nrt_periodic_thread_body(void *arg) {
    struct periodic_task *ptask;
    struct th_info* the_thread = (struct th_info*) arg;
    struct threadargs threadargs;

    /* ptask = start_periodic_timer(2000000, the_thread->period); */
    ptask = start_periodic_timer(0, the_thread->period);
    if (ptask == NULL) {
        printf("Start Periodic Timer");

        return NULL;
    }

    while(1) {
        wait_next_activation(ptask);
        the_thread->body((void*) &threadargs);
    }

    return NULL;
}

static void fail(const char *reason) {
    perror(reason);
    exit(EXIT_FAILURE);
}

struct periodic_task *start_periodic_timer(unsigned long long offset_in_us,
        int period) {
    struct periodic_task *ptask;

    ptask = (struct periodic_task*) malloc(sizeof(struct periodic_task));
    if (ptask == NULL) {
        return NULL;
    }

    // Current time is added first, we let the thread wait for its next
    // activation using absolute time.
    clock_gettime(CLOCK_REALTIME, &ptask->ts);
    timespec_add_us(&ptask->ts, offset_in_us);
    ptask->period = period;
    return ptask;
}

void wait_next_activation(struct periodic_task *ptask) {
    // Suspend the thread until the time value specified by &t->ts has elapsed.
    // Note the use of absolute time.
    clock_nanosleep(CLOCK_REALTIME, TIMER_ABSTIME, &ptask->ts, NULL);
    // Add another period to the time specification.
    timespec_add_us(&ptask->ts, ptask->period);
}

void dzn_light_led(char* color) {
    // Set color
    int r = safe_call([=]() { strcpy(::color, color); }, &mutex["color"]);

    if (r != OK) exit(EXIT_FAILURE);
    // Let rt_light_led know that it can send a color to nrt_light_led.
    sem_post(&semaphore["led"]);
}
