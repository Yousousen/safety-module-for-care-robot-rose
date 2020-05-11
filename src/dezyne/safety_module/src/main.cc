// TODO: Clean up the entire code. Order it logically.
// Safety module
// The module contains a pointer to a care robot, which provides the module
// with a representation of a care robot that is useful to it.  In the file,
// retrieve functions update data from a sensor.
#include <cstdlib>
#include <ctime>
#include <iostream>
#include <iomanip>
#include <string>
#include <thread>
#include <chrono>

// for framebuffer control
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

#include <linux/input.h>
#include <linux/fb.h>

// For Dezyne
#include <dzn/runtime.hh>
#include <dzn/locator.hh>
#include "System.hh"

#include <signal.h>
#include <pthread.h>
#include <errno.h>


#include "constants.hh"
#include "calculus.hh"
#include "physics.hh"
#include "care_robot.hh"

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

/*** data retrieval functions ***/
void retrieve_position();
void retrieve_velocity();
double retrieve_acceleration();
void retrieve_angular_velocity();
void retrieve_angular_acceleration();
void retrieve_all();

/*** data set functions ***/
void set_acceleration(double acceleration);

bool robot_is_moving();
bool arm_is_folded();
bool arm_has_payload();

/*** sampling functions ***/
void sample_acceleration(double* f, const int nsamples);
void sample_angular_acceleration(double* f, const int nsamples);

/*** retrievers and resolvers ***/
void retrieve_ke_from_acc();
Behavior::type resolve_ke_from_acc();
void retrieve_re_from_ang_acc();
Behavior::type resolve_re_from_ang_acc();
void retrieve_arm_str();
Behavior::type resolve_arm_str();
void retrieve_arm_pos();
Behavior::type resolve_arm_pos();

void initialise();
void destruct();

void what_triggered(bool acc, bool angacc, bool str, bool pos);

/*** Threads related ***/

/** Real-time threads **/
// Sends colors to nrt_light_led.
static void* rt_light_led(void* arg);


// sets the acceleration, retrieves acceleration from
// nrt_retrieve_acceleration.
static void* rt_retrieve_acceleration(void* arg);


// thread to sample acceleration continuously.
// Currently does the following (TODO: probably split):
// - set number of samples
// - samples acceleration
// - ∫ a dt
// - calculates KE
static void* rt_sample_acceleration(void* arg);

// Starts a periodic real-time thread.
static void *rt_periodic_thread_body(void *arg);


/** Non-real-time threads **/
// The LED matrix driver.
// blocks on read to read a color from the XDDP socket, indefinitely.
static void* nrt_light_led(void *arg);

// The acceleration driver.
// sends current acceleration over XDDP socket to rt_retrieve_acceleration.
static void* nrt_retrieve_acceleration(void *arg);

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

// Add period microseconds to timespec ts.
static inline void timespec_add_us(struct timespec *ts, unsigned long long period)
{
    // (1µs) = 1000 * (1ns)
    period *= 1000;
    // Add number of nanoseconds already in ts to period.
    period += ts->tv_nsec;
    // To set tv_sec to the correct number we look at how many seconds of
    // nanoseconds there are in period.
    while (period >= NSEC_PER_SEC) {
        period -= NSEC_PER_SEC;
	ts->tv_sec += 1;
    }
    // The nanoseconds left.
    ts->tv_nsec = period;
}


/*** Global variables ***/
struct Position* position = nullptr;
struct Velocity* velocity = nullptr;
struct Acceleration* acceleration = nullptr;
struct AngularVelocity* angular_velocity = nullptr;
struct AngularAcceleration* angular_acceleration = nullptr;

double kinetic_energy;
double rotational_energy;
double arm_strength;
int arm_position;

CareRobotRose* rose = nullptr;

// For LED Matrix 
struct fb_t *fb;
int fbfd = 0;

// Semaphores
sem_t sem_led;
sem_t sem_ret_acc;
sem_t sem_ret_angacc;
sem_t sem_ret_str;
sem_t sem_ret_pos;
sem_t sem_sample_acc;

// Mutexes
pthread_mutex_t mutex_color;
pthread_mutex_t mutex_acc;
pthread_mutex_t mutex_angacc;
pthread_mutex_t mutex_str;
pthread_mutex_t mutex_pos;
pthread_mutex_t mutex_kinetic_energy;
pthread_mutex_t mutex_rotational_energy;
pthread_mutex_t mutex_arm_strength;
pthread_mutex_t mutex_arm_position;

static char color[SIZE];

/* static const char* color_blue = "0x0006"; */
/* static const char* color_red = "0x3000"; */

auto main() -> int {
    initialise();
    int r = roll();
    if (r != OK) return EXIT_FAILURE;
    destruct();
}

ErrorCode_t roll() {
    // Initialise dezyne locator and runtime.
    IResolver iResolver({});

    // Bind resolvers
    iResolver.in.resolve_ke_from_acc = []() -> Behavior::type {
        Behavior::type type;
        double ke;

        if (0 != (errno = pthread_mutex_lock(&mutex_kinetic_energy))) { // Lock
            perror("pthread_mutex_lock failed");
            exit(EXIT_FAILURE);
        }

        ke = kinetic_energy;

        if (0 != (errno = pthread_mutex_unlock(&mutex_kinetic_energy))) { // Unlock
            perror("pthread_mutex_unlock failed");
            exit(EXIT_FAILURE);
        }

        if (ke > MAX_KE)
            type = Behavior::type::Unsafe;
        else
            type = Behavior::type::Safe;
        return type;
    };

    iResolver.in.resolve_re_from_ang_acc = []() -> Behavior::type {
        Behavior::type type;
        double re;

        if (0 != (errno = pthread_mutex_lock(&mutex_rotational_energy))) { // Lock
            perror("pthread_mutex_lock failed");
            exit(EXIT_FAILURE);
        }

        re = rotational_energy;

        if (0 != (errno = pthread_mutex_unlock(&mutex_rotational_energy))) { // Unlock
            perror("pthread_mutex_unlock failed");
            exit(EXIT_FAILURE);
        }

        if (re > MAX_RE)
            type =  Behavior::type::Unsafe;
        else
            type =  Behavior::type::Safe;
        return type;
    };

    iResolver.in.resolve_arm_str = []() -> Behavior::type {
        Behavior::type type;
        double str;

        if (0 != (errno = pthread_mutex_lock(&mutex_arm_strength))) { // Lock
            perror("pthread_mutex_lock failed");
            exit(EXIT_FAILURE);
        }

        str = arm_strength;

        if (0 != (errno = pthread_mutex_unlock(&mutex_arm_strength))) { // Unlock
            perror("pthread_mutex_unlock failed");
            exit(EXIT_FAILURE);
        }

        if (arm_has_payload() && str > MAX_STR_PAYLOAD)
            type = Behavior::type::Unsafe;
        else if (str > MAX_STR)
            type = Behavior::type::Unsafe;
        else
            type = Behavior::type::Safe;
        return type;
    };

    iResolver.in.resolve_arm_pos = []() -> Behavior::type {
        Behavior::type type;

        if (0 != (errno = pthread_mutex_lock(&mutex_arm_position))) { // Lock
            perror("pthread_mutex_lock failed");
            exit(EXIT_FAILURE);
        }

        if (robot_is_moving() && !arm_is_folded())
            type = Behavior::type::Unsafe;
        else
            type = Behavior::type::Safe;

        if (0 != (errno = pthread_mutex_unlock(&mutex_arm_position))) { // Unlock
            perror("pthread_mutex_unlock failed");
            exit(EXIT_FAILURE);
        }
        return type;
    };

    dzn::locator locator;
    dzn::runtime runtime;
    locator.set(runtime);
    locator.set(iResolver);

    System s(locator);
    s.dzn_meta.name = "System";


    /*** Bind native functions ***/
    s.iController.out.what_triggered = what_triggered;

    s.iLEDControl.in.initialise_framebuffer = initialise_framebuffer;
    s.iLEDControl.in.destruct_framebuffer = destruct_framebuffer;
    s.iLEDControl.in.light_led_red = dzn_light_led;
    s.iLEDControl.in.light_led_blue = dzn_light_led;
    s.iLEDControl.in.reset_led = reset_led;

    s.iAccelerationSensor.in.retrieve_ke_from_acc = retrieve_ke_from_acc;
    s.iAngularAccelerationSensor.in.retrieve_re_from_ang_acc = retrieve_re_from_ang_acc;

    s.iGripArmSensor.in.retrieve_arm_str = retrieve_arm_str;
    s.iGripArmSensor.in.retrieve_arm_pos = retrieve_arm_pos;


    // Check bindings
    s.check_bindings();


    // Initialise framebuffer
    s.iController.in.initialise();

    /*** Threads ***/
    struct sched_param rtparam = { .sched_priority = 42 };
    pthread_attr_t rtattr, nrtattr;
    sigset_t set;
    int sig;
    static pthread_t th_rt_light_led, th_rt_ret_acc, th_rt_sample_acc;
    static pthread_t th_nrt_light_led, th_nrt_ret_acc;
    static struct th_info rt_info;
    struct threadargs thread_args;

    sigemptyset(&set);
    sigaddset(&set, SIGINT);
    sigaddset(&set, SIGTERM);
    sigaddset(&set, SIGHUP);

    pthread_attr_init(&nrtattr);
    pthread_attr_setdetachstate(&nrtattr, PTHREAD_CREATE_JOINABLE);
    pthread_attr_setinheritsched(&nrtattr, PTHREAD_EXPLICIT_SCHED);
    pthread_attr_setschedpolicy(&nrtattr, SCHED_OTHER);
    pthread_attr_setschedparam(&rtattr, &rtparam);

    // Initialise semaphores
    // params: the semaphore, share between threads, initialise with value 0.
    sem_init(&sem_led, 0, 0);
    sem_init(&sem_ret_acc, 0, 0);
    sem_init(&sem_ret_angacc, 0, 0); 
    sem_init(&sem_ret_str, 0, 0); 
    sem_init(&sem_ret_pos, 0, 0); 
    sem_init(&sem_sample_acc, 0, 0); 


    // Initialise mutexes.
    pthread_mutex_t mutex_fb;
    if (0 != (errno = pthread_mutex_init(&mutex_fb, NULL))) {
        perror("pthread_mutex_init() failed");
        return NOT_OK;
    }
    thread_args.mutex_fb = &mutex_fb;

    if (0 != (errno = pthread_mutex_init(&mutex_color, NULL))) {
        perror("pthread_mutex_init() failed");
        return NOT_OK;
    }
    thread_args.mutex_color = &mutex_color;

    if (0 != (errno = pthread_mutex_init(&mutex_acc, NULL))) {
        perror("pthread_mutex_init() failed");
        return NOT_OK;
    }
    thread_args.mutex_acc = &mutex_acc;

    if (0 != (errno = pthread_mutex_init(&mutex_angacc, NULL))) {
        perror("pthread_mutex_init() failed");
        return NOT_OK;
    }
    thread_args.mutex_angacc = &mutex_angacc;

    if (0 != (errno = pthread_mutex_init(&mutex_str, NULL))) {
        perror("pthread_mutex_init() failed");
        return NOT_OK;
    }
    thread_args.mutex_str = &mutex_str;

    if (0 != (errno = pthread_mutex_init(&mutex_pos, NULL))) {
        perror("pthread_mutex_init() failed");
        return NOT_OK;
    }
    thread_args.mutex_pos = &mutex_pos;

    /*** Start threads ***/
    // Start thread rt_light_led
    errno = pthread_create(&th_rt_light_led, &rtattr, &rt_light_led, NULL);
    if (errno)
        fail("pthread_create");

    // Start thread nrt_light_led
    errno = pthread_create(&th_nrt_light_led, &nrtattr, &nrt_light_led, (void*) &thread_args);
    if (errno)
        fail("pthread_create");

    // Start thread rt_retrieve_acceleration
    errno = pthread_create(&th_rt_ret_acc, &rtattr, &rt_retrieve_acceleration, &thread_args);
    if (errno)
        fail("pthread_create");

    // Start thread nrt_retrieve_acceleration
    errno = pthread_create(&th_nrt_ret_acc, &nrtattr, &nrt_retrieve_acceleration, NULL);
    if (errno)
        fail("pthread_create");

    // Start thread rt_sample_acceleration
    errno = pthread_create(&th_rt_sample_acc, &rtattr, &rt_sample_acceleration,
            &thread_args);
    if (errno)
        fail("pthread_create");

    printf("Started running indefinitely.\n");
    std::string input;
    while (1) {
        printf("press: q to quit, d to execute all checks, r to reset\n");
        printf("a to check acc, aa to check ang acc, s to check str, p to check pos\n\n> ");

        // Notify nrt_retrieve_acceleration that it can retrieve acceleration.
        sem_post(&sem_ret_acc);
        // Notify rt_sample_acceleration that it can go sample acceleration.
        /* sem_post(&sem_sample_acc); */

        std::cin >> input;
        /* input = "a"; */
        if (input == "q") {
            break;
        } else if (input == "d") {
            s.iController.in.do_checks();
        } else if (input == "a") {
            s.iController.in.check_acc();
        } else if (input == "aa") {
            s.iController.in.check_angacc();
        } else if (input == "s") {
            s.iController.in.check_str();
        } else if (input == "p") {
            s.iController.in.check_pos();
        } else if (input == "r") {
            s.iController.in.reset();
        } else if (input == "i") {
            // Purposely here to show illegal exception handler.
            s.iController.in.initialise();
        } else {
            printf("Did not understand input.\n");
        }
    }
    printf("Stopping\n");

    // Destruct framebuffer
    s.iController.in.destruct();

    // Kill threads
    pthread_cancel(th_nrt_light_led);
    pthread_cancel(th_nrt_ret_acc);
    pthread_cancel(th_rt_light_led);
    pthread_cancel(th_rt_ret_acc);
    pthread_cancel(th_rt_sample_acc);

    pthread_join(th_nrt_light_led, NULL);
    pthread_join(th_nrt_ret_acc, NULL);
    pthread_join(th_rt_light_led, NULL);
    pthread_join(th_rt_ret_acc, NULL);
    pthread_join(th_rt_sample_acc, NULL);

    // Destroy mutexes
    if (0 != (errno = pthread_mutex_destroy(&mutex_fb))) {
        perror("pthread_mutex_destroy() failed");
        return NOT_OK;
    }

    if (0 != (errno = pthread_mutex_destroy(&mutex_color))) {
        perror("pthread_mutex_destroy() failed");
        return NOT_OK;
    }

    if (0 != (errno = pthread_mutex_destroy(&mutex_acc))) {
        perror("pthread_mutex_destroy() failed");
        return NOT_OK;
    }

    if (0 != (errno = pthread_mutex_destroy(&mutex_angacc))) {
        perror("pthread_mutex_destroy() failed");
        return NOT_OK;
    }

    if (0 != (errno = pthread_mutex_destroy(&mutex_str))) {
        perror("pthread_mutex_destroy() failed");
        return NOT_OK;
    }

    if (0 != (errno = pthread_mutex_destroy(&mutex_pos))) {
        perror("pthread_mutex_destroy() failed");
        return NOT_OK;
    }

    // Destroy semaphores
    sem_destroy(&sem_led); 
    sem_destroy(&sem_ret_acc); 
    sem_destroy(&sem_ret_angacc); 
    sem_destroy(&sem_ret_str); 
    sem_destroy(&sem_ret_pos); 
    sem_destroy(&sem_sample_acc); 

    return OK;
}


void initialise() {
    // Create a rose representation
    rose = new CareRobotRose();

    if (CSV_HAS_POSITION) {
        // Load from CSV.
    } else {
        position = new Position(11.0, 12.0, 13.0, 14.0);
    }

    if (CSV_HAS_VELOCITY) {
        // Load from CSV.
    } else {
        velocity = new Velocity(11.0, 12.0, 13.0, 14.0);
    }

    if (CSV_HAS_ACCELERATION) {
        // Load from CSV.
    } else {
        acceleration = new Acceleration(11.0, 12.0, 13.0, 14.0);
    }

    if (CSV_HAS_ANGULAR_ACCELERATION) {
        // Load from CSV.
    } else {
        angular_acceleration = new AngularAcceleration(11.0, 12.0, 13.0, 14.0);
    }

    if (CSV_HAS_ANGULAR_VELOCITY) {
        // Load from CSV.
    } else {
        angular_velocity = new AngularVelocity(11.0, 12.0, 13.0, 14.0);
    }

    if (CSV_HAS_ANGULAR_ACCELERATION) {
        // Load from CSV.
    } else {
        angular_acceleration = new AngularAcceleration(11.0, 12.0, 13.0, 14.0);
    }

}

void destruct() {
    if (rose != nullptr)                 delete rose;
    if (position != nullptr)             delete position;
    if (velocity != nullptr)             delete velocity;
    if (acceleration != nullptr)         delete acceleration;
    if (angular_velocity != nullptr)     delete angular_velocity;
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
    memset(fb, 0, 128);
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


void retrieve_position() {
    auto previous = position->current;
    srand((unsigned)time(NULL));
    position->current = rand() % 5 + 1;
    position->change = position->current - previous;
}

void retrieve_velocity() {
    auto previous = velocity->current;
    srand((unsigned)time(NULL));
    velocity->current = rand() % 5 + 1;
    velocity->change = velocity->current - previous;
}

double retrieve_acceleration() {
    srand((unsigned)time(NULL));
    return rand() % 5 + 1;
}

void set_acceleration(double acceleration) {
    auto previous = ::acceleration->current;
    ::acceleration->current = acceleration;
    ::acceleration->change = ::acceleration->current - previous;
}

void retrieve_angular_velocity() {
    auto previous = angular_velocity->current;
    srand((unsigned)time(NULL));
    angular_velocity->current = rand() % 5 + 1;
    angular_velocity->change = angular_velocity->current - previous;
}


void retrieve_angular_acceleration() {
    auto previous = angular_acceleration->current;
    srand((unsigned)time(NULL));
    angular_acceleration->current = rand() % 5 + 1;
    angular_acceleration->change = angular_acceleration->current -
        previous;
}

void retrieve_all() {
    retrieve_position();
    retrieve_velocity();
    retrieve_acceleration();
    retrieve_angular_velocity();
    retrieve_angular_acceleration();
}

void sample_acceleration(double* f, const int nsamples) {
    printf("# rectangles: ");
    int i;
    for (i = 0; i < nsamples; ++i) {
        // Get acceleration
        if (0 != (errno = pthread_mutex_lock(&mutex_acc))) { // Lock
            perror("pthread_mutex_lock failed");
            exit(EXIT_FAILURE);
        }

        f[i] = acceleration->current;

        // Unlock
        if (0 != (errno = pthread_mutex_unlock(&mutex_acc))) { // Unlock
            perror("pthread_mutex_unlock failed");
            exit(EXIT_FAILURE);
        }

        /* std::this_thread::sleep_for(std::chrono::milliseconds(CHANGE_IN_TIME_MS)); */
        std::this_thread::sleep_for(std::chrono::microseconds(CHANGE_IN_TIME_MICRO));
    }
    printf("%d, ", i);
}


void sample_angular_acceleration(double* f, const int nsamples) {
    printf("# rectangles: ");
    int i;
    for (i = 0; i < nsamples; ++i) {
        retrieve_angular_acceleration();
        f[i] = angular_acceleration->current;
        /* std::this_thread::sleep_for(std::chrono::milliseconds(CHANGE_IN_TIME_MS)); */
        std::this_thread::sleep_for(std::chrono::microseconds(CHANGE_IN_TIME_MICRO));
    }
    printf("%d, ", i);
}


void retrieve_ke_from_acc() {
    // Currently rt_sample_acceleration sets kinetic energy, so wee need not do
    // anything here.
}

void retrieve_re_from_ang_acc() {
    // numbers of seconds to sample.
    const double nseconds = 0.5;
    // numbers of samples.
    const int nsamples = (int) (nseconds / CHANGE_IN_TIME);
    printf("nsamples: %d\n", nsamples);

    // Save acceleration in here.
    double a[nsamples];

    printf(">>> sampling angular acceleration\n\n");
    sample_angular_acceleration(a, nsamples);
    printf("<<< done\n\n");

    // Numerical integration
    double angular_velocity = Calculus::trapezoidal_integral(a, nsamples);
    printf("angular velocity = %f\n", angular_velocity);

    // Calculate kinetic energy
    rotational_energy = 0.5 * INERTIAL_MASS * angular_velocity * angular_velocity;
    printf("rotational energy = %f\n", rotational_energy);
}

void retrieve_arm_str() {
    printf(">>> retrieving grip arm strength\n\n");
    rose->arm->retrieve_strength();
    arm_strength = rose->arm->current_strength;
    // Unneeded, just for slow output.
    std::this_thread::sleep_for(std::chrono::microseconds(500));
    printf("Grip arm strength: %f\n", arm_strength);
    printf("<<< done\n\n");
}

void retrieve_arm_pos() {
    printf(">>> retrieving grip arm position\n\n");
    rose->arm->retrieve_position();
    arm_position = rose->arm->current_position;
    // Unneeded, just for slow output.
    std::this_thread::sleep_for(std::chrono::microseconds(500));
    printf("Grip arm position: %d\n", arm_position);
    printf("<<< done\n\n");
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

void what_triggered(bool acc, bool angacc, bool str, bool pos) {
    if (!(acc || angacc || str || pos)) return;
    printf(">>> what triggered:\n\n");
    printf("Unsafe behavior caused by:\n");
    if (acc)    printf("acceleration\n");
    if (angacc) printf("angular acceleration\n");
    if (str)    printf("arm strength\n");
    if (pos)    printf("arm position\n");
    printf("<<<\n\n");
}

static void* rt_light_led(void* arg) {
    int n = 0;
    int len;
    int ret;
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
        sem_wait(&sem_led); 
        char msg[SIZE];

        // Get color
        if (0 != (errno = pthread_mutex_lock(&mutex_color))) { // Lock
            perror("pthread_mutex_lock failed");
            exit(EXIT_FAILURE);
        }

        strcpy(msg, color);

        // Unlock
        if (0 != (errno = pthread_mutex_unlock(&mutex_color))) { // Unlock
            perror("pthread_mutex_unlock failed");
            exit(EXIT_FAILURE);
        }

        len = strlen(msg);
        ret = sendto(s, msg, len, 0, NULL, 0);
        if (ret != len)
            fail("sendto");
        printf("%s: sent %d bytes, \"%.*s\"\n", __FUNCTION__, ret, ret, msg);
    }
    return NULL;
}

static void* nrt_light_led(void *arg) {
    struct threadargs *args = (struct threadargs *)arg;
    // Pointer to mutex is passed as an argument.
    pthread_mutex_t* mutex_fb = args->mutex_fb;

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

        // Lock
        if (0 != (errno = pthread_mutex_lock(mutex_fb))) {
            perror("pthread_mutex_lock failed");
            exit(EXIT_FAILURE);
        }

        // Color the matrix.
        light_led(color);

        // Unlock
        if (0 != (errno = pthread_mutex_unlock(mutex_fb))) {
            perror("pthread_mutex_unlock failed");
            exit(EXIT_FAILURE);
        }
    }

    return NULL;
}

static void* rt_retrieve_acceleration(void* arg) {
    int n = 0;
    int len;
    int ret;
    int s;
    char buf[BUFSIZE];
    struct sockaddr_ipc saddr;
    size_t poolsz;

    struct threadargs *args = (struct threadargs *)arg;
    // Pointer to mutex is passed as an argument.
    pthread_mutex_t* mutex_acc = args->mutex_acc;

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
     * Send a datagram to the NRT endpoint via the proxy.
     * We may pass a NULL destination address, since a
     * bound socket is assigned a default destination
     * address matching the binding address (unless
     * connect(2) was issued before bind(2), in which case
     * the former would prevail).
     */
    while (1) {
        /* Read packets echoed by the non-real-time thread */
        ret = recvfrom(s, buf, sizeof(buf), 0, NULL, 0);
        if (ret <= 0)
            fail("recvfrom");
        /* printf("   => \"%.*s\" received by peer\n", ret, buf); */
        n = (n + 1) % (sizeof(buf) / sizeof(buf[0]));

        // Set acceleration
        if (0 != (errno = pthread_mutex_lock(mutex_acc))) { // Lock
            perror("pthread_mutex_lock failed");
            exit(EXIT_FAILURE);
        }

        set_acceleration(atof(buf));

        // Unlock
        if (0 != (errno = pthread_mutex_unlock(mutex_acc))) { // Unlock
            perror("pthread_mutex_unlock failed");
            exit(EXIT_FAILURE);
        }
    }
    return NULL;
}

static void* nrt_retrieve_acceleration(void *arg) {
    /* struct threadargs *args = (struct threadargs *)arg; */
    // Pointer to mutex is passed as an argument.
    /* pthread_mutex_t* mutex_acc = args->mutex_acc; */

    char buf[BUFSIZE], *devname;
    int fd, ret;
    if (asprintf(&devname, "/dev/rtp%d", XDDP_PORT_RET_ACC) < 0)
        fail("asprintf");
    fd = open(devname, O_RDWR);
    free(devname);
    if (fd < 0)
        fail("open");

    while (1) {
        // Wait for an up before retrieving acceleration from the sense hat
        // driver.
        sem_wait(&sem_ret_acc);

        // Retrieve acceleration
        double acc = retrieve_acceleration();
        snprintf(buf, BUFSIZE, "%f", acc);

        /* Write retrieved acceleration to rt_retrieve_acceleration. */
        ret = write(fd, buf, BUFSIZE);
        if (ret <= 0)
            fail("write");
    }

    return NULL;
}

static void* rt_sample_acceleration(void* arg) {
    // numbers of seconds to sample.
    const double nseconds = 0.5;
    // numbers of samples.
    const int nsamples = (int) (nseconds / CHANGE_IN_TIME);
    /* printf("nsamples: %d\n", nsamples); */

    // Save acceleration in here.
    double a[nsamples];

    while (1) {
        /* sem_wait(&sem_sample_acc); */
        /* printf(">>> sampling acceleration\n\n"); */
        /* printf("# rectangles: "); */
        for (int i = 0; i < nsamples; ++i) {
            // Get acceleration
            if (0 != (errno = pthread_mutex_lock(&mutex_acc))) { // Lock
                perror("pthread_mutex_lock failed");
                exit(EXIT_FAILURE);
            }

            a[i] = acceleration->current;

            // Unlock
            if (0 != (errno = pthread_mutex_unlock(&mutex_acc))) { // Unlock
                perror("pthread_mutex_unlock failed");
                exit(EXIT_FAILURE);
            }

            /* std::this_thread::sleep_for(std::chrono::milliseconds(CHANGE_IN_TIME_MS)); */
            std::this_thread::sleep_for(std::chrono::microseconds(CHANGE_IN_TIME_MICRO));
            /* printf("%d, ", i); */
        }
        /* printf("\n"); */
        /* printf("\n<<< sampling acceleration done\n\n"); */

        // Numerical integration
        double velocity = Calculus::trapezoidal_integral(a, nsamples);
        /* printf("velocity = %f\n", velocity); */

        // Calculate kinetic energy
        if (0 != (errno = pthread_mutex_lock(&mutex_kinetic_energy))) { // Lock
            perror("pthread_mutex_lock failed");
            exit(EXIT_FAILURE);
        }

        kinetic_energy = 0.5 * INERTIAL_MASS * velocity * velocity;
        printf("kinetic energy = %f\n", kinetic_energy);

        if (0 != (errno = pthread_mutex_unlock(&mutex_kinetic_energy))) { // Unlock
            perror("pthread_mutex_unlock failed");
            exit(EXIT_FAILURE);
        }
    }
}

static void *rt_periodic_thread_body(void *arg) {
    struct periodic_task* ptask;
    struct th_info* the_thread = (struct th_info*) arg;
    struct threadargs rt_args;

    /* ptask = start_periodic_timer(2000000, the_thread->period); */
    ptask = start_periodic_timer(0, the_thread->period);
    if (ptask == NULL) {
        printf("Start Periodic Timer");

        return NULL;
    }

    while(1) {
        wait_next_activation(ptask);
        the_thread->body((void*) &rt_args);
    }

    return NULL;
}

static void *nrt_periodic_thread_body(void *arg) {
    struct periodic_task *ptask;
    struct th_info* the_thread = (struct th_info*) arg;
    struct threadargs thread_args;

    /* ptask = start_periodic_timer(2000000, the_thread->period); */
    ptask = start_periodic_timer(0, the_thread->period);
    if (ptask == NULL) {
        printf("Start Periodic Timer");

        return NULL;
    }

    while(1) {
        wait_next_activation(ptask);
        the_thread->body((void*) &thread_args);
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

    clock_gettime(CLOCK_REALTIME, &ptask->ts);
    timespec_add_us(&ptask->ts, offset_in_us);
    ptask->period = period;

    return ptask;
}

void wait_next_activation(struct periodic_task *ptask) {
    // Suspend the thread until the time value specified by &t->ts has elapsed.
    clock_nanosleep(CLOCK_REALTIME, TIMER_ABSTIME, &ptask->ts, NULL);
    // Add another period to the time specification.
    timespec_add_us(&ptask->ts, ptask->period);
}


void dzn_light_led(char* color) {
    // Set color
    if (0 != (errno = pthread_mutex_lock(&mutex_color))) { // Lock
        perror("pthread_mutex_lock failed");
        exit(EXIT_FAILURE);
    }

    strcpy(::color, color);

    // Unlock
    if (0 != (errno = pthread_mutex_unlock(&mutex_color))) { // Unlock
        perror("pthread_mutex_unlock failed");
        exit(EXIT_FAILURE);
    }
    // Let rt_light_led know that it can send a color to nrt_light_led.
    sem_post(&sem_led);
}

