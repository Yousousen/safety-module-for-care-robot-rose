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

#include <linux/input.h>
#include <linux/fb.h>

// For Dezyne
#include <dzn/runtime.hh>
#include <dzn/locator.hh>
#include "System.hh"

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
// overloads of light_led function
void light_led_red();
void light_led_blue();
// Reset the led matrix to low.
void reset_led();

/*** data retrieval functions ***/
void retrieve_position();
void retrieve_velocity();
void retrieve_acceleration();
void retrieve_angular_velocity();
void retrieve_angular_acceleration();
void retrieve_all();

bool is_moving();
bool arm_is_folded();
bool arm_has_payload();

/*** sampling functions ***/
void sample_acceleration(double* f, const int nsamples);

/*** retrievers and resolvers ***/
void retrieve_ke_from_acc();
Behavior::type resolve_ke_from_acc();
void retrieve_re_from_ang_acc();
Behavior::type resolve_re_from_ang_acc();

void initialise();
void destruct();


/*** Global variables ***/
struct Position* position = nullptr;
struct Velocity* velocity = nullptr;
struct Acceleration* acceleration = nullptr;
struct AngularVelocity* angular_velocity = nullptr;
struct AngularAcceleration* angular_acceleration = nullptr;

double kinetic_energy;
double rotational_energy;
double grip_strength;

CareRobotRose* rose = nullptr;

// For LED Matrix 
struct fb_t *fb;
ErrorCode_t ret = OK;
int fbfd = 0;




auto main() -> int {
    initialise();
    roll();
    destruct();
}

ErrorCode_t roll() {
    // Initialise dezyne locator and runtime.
    IResolver iResolver({});
    iResolver.in.resolve_ke_from_acc = resolve_ke_from_acc;
    iResolver.in.resolve_re_from_ang_acc = resolve_re_from_ang_acc;

    dzn::locator locator;
    dzn::runtime runtime;
    locator.set(runtime);
    locator.set(iResolver);

    System s(locator);
    s.dzn_meta.name = "System";


    /*** Bind native functions ***/
    s.iLEDControl.in.initialise_framebuffer = initialise_framebuffer;
    s.iLEDControl.in.destruct_framebuffer = destruct_framebuffer;
    s.iLEDControl.in.light_led_red = light_led_red;
    s.iLEDControl.in.light_led_blue = light_led_blue;
    s.iLEDControl.in.reset_led = reset_led;
    s.iAccelerationSensor.in.retrieve_ke_from_acc = retrieve_ke_from_acc;
    s.iAngularAccelerationSensor.in.retrieve_re_from_ang_acc = retrieve_re_from_ang_acc;

    // Check bindings
    s.check_bindings();


    // Initialise framebuffer
    s.iController.in.initialise();

    // Run indefinitely unless input is equal to "q".
#if REALTIME
    rt_printf("Started running indefinitely.\n");
#else
    printf("Started running indefinitely.\n");
#endif
    std::string input;
    while (1) {
#if REALTIME
        rt_printf("\n");
#else
        printf("press: q to quit, d to execute checks, r to reset\n");
#endif
        std::cin >> input;
        /* input = "a"; */
        if (input == "q") {
            break;
        /* } else if (input == "r") { // for debugging */
            /* s.iController.in.light_red(); */
        /* } else if (input == "b") { */
            /* s.iController.in.light_blue(); // for debugging */
        } else if (input == "d") {
            // Used to pass around kinetic energy from retriever to resolver in
            // dezyne.
            s.iController.in.do_checks();
        } else if (input == "r") {
            s.iController.in.reset();
        } else if (input == "i") {
            // Purposely here to show illegal exception handler.
            s.iController.in.initialise();
        } else {
#if REALTIME
            rt_printf("Did not understand input.\n");
#else
            printf("Did not understand input.\n");
#endif
        }
    }
    // Destruct framebuffer
    s.iController.in.destruct();
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

void light_led_red() {
    light_led(LedColor_t::RED);
}

void light_led_blue() {
    light_led(LedColor_t::BLUE);
}

void reset_led() {
    memset(fb, 0, 128);
}

void initialise_framebuffer() {

    // Initialise frame buffer for led matrix.
#if REALTIME
    rt_printf("Initialising framebuffer...\n");
#else
    printf("Initialising framebuffer...\n");
#endif
    fbfd = open_fbdev("RPi-Sense FB");
    if (fbfd <= 0) {
        ret = NOT_OK;
#if REALTIME
        rt_printf("Error: cannot open framebuffer device.\n");
#else
        printf("Error: cannot open framebuffer device.\n");
#endif
        return;
    }
    fb = (struct fb_t*) mmap(0, 128, PROT_READ | PROT_WRITE, MAP_SHARED, fbfd, 0);
    if (!fb) {
        ret = NOT_OK;
#if REALTIME
        rt_printf("Failed to mmap.\n");
#else
        printf("Failed to mmap.\n");
#endif
        return;
    }
    memset(fb, 0, 128);
#if REALTIME
    rt_printf("Framebuffer initialised.\n");
#else
    printf("Framebuffer initialised.\n");
#endif
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

void retrieve_acceleration() {
    auto previous = acceleration->current;
    srand((unsigned)time(NULL));
    acceleration->current = rand() % 5 + 1;
    acceleration->change = acceleration->current - previous;
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
#if REALTIME
    rt_printf("# rectangles: ");
#else
    printf("# rectangles: ");
#endif
    int i;
    for (i = 0; i < nsamples; ++i) {
        retrieve_acceleration();
        f[i] = acceleration->current;
        /* std::this_thread::sleep_for(std::chrono::milliseconds(CHANGE_IN_TIME_MS)); */
        std::this_thread::sleep_for(std::chrono::microseconds(CHANGE_IN_TIME_MICRO));
    }
#if REALTIME
    rt_printf("%d, ", i);
#else
    printf("%d, ", i);
#endif
}


void sample_angular_acceleration(double* f, const int nsamples) {
#if REALTIME
    rt_printf("# rectangles: ");
#else
    printf("# rectangles: ");
#endif
    int i;
    for (i = 0; i < nsamples; ++i) {
        retrieve_angular_acceleration();
        f[i] = angular_acceleration->current;
        /* std::this_thread::sleep_for(std::chrono::milliseconds(CHANGE_IN_TIME_MS)); */
        std::this_thread::sleep_for(std::chrono::microseconds(CHANGE_IN_TIME_MICRO));
    }
#if REALTIME
    rt_printf("%d, ", i);
#else
    printf("%d, ", i);
#endif
}


void retrieve_ke_from_acc() {
    // numbers of seconds to sample.
    const double nseconds = 0.5;
    // numbers of samples.
    const int nsamples = (int) (nseconds / CHANGE_IN_TIME);
#if REALTIME
    rt_printf("nsamples: %d\n", nsamples);
#else
    printf("nsamples: %d\n", nsamples);
#endif

    // Save acceleration in here.
    double a[nsamples];

#if REALTIME
    rt_printf("SAMPLING ACCELERATION\n");
#else
    printf("SAMPLING ACCELERATION\n");
#endif
    sample_acceleration(a, nsamples);
#if REALTIME
    rt_printf("DONE\n");
#else
    printf("DONE\n");
#endif

    // Numerical integration
    double velocity = Calculus::trapezoidal_integral(a, nsamples);
#if REALTIME
    rt_printf("velocity = %f\n", velocity);
#else
    printf("velocity = %f\n", velocity);
#endif

    // Calculate kinetic energy
    kinetic_energy = 0.5 * INERTIAL_MASS * velocity * velocity;
#if REALTIME
    rt_printf("kinetic energy = %f\n", kinetic_energy);
#else
    printf("kinetic energy = %f\n", kinetic_energy);
#endif
}

Behavior::type resolve_ke_from_acc() {
    Behavior::type behavior = (kinetic_energy > MAX_KE) ? Behavior::type::Unsafe :
        Behavior::type::Safe;
    return behavior;
}

void retrieve_re_from_ang_acc() {
    // numbers of seconds to sample.
    const double nseconds = 0.5;
    // numbers of samples.
    const int nsamples = (int) (nseconds / CHANGE_IN_TIME);
#if REALTIME
    rt_printf("nsamples: %d\n", nsamples);
#else
    printf("nsamples: %d\n", nsamples);
#endif

    // Save acceleration in here.
    double a[nsamples];

#if REALTIME
    rt_printf("SAMPLING ANGULAR ACCELERATION\n");
#else
    printf("SAMPLING ANGULAR ACCELERATION\n");
#endif
    sample_angular_acceleration(a, nsamples);
#if REALTIME
    rt_printf("DONE\n");
#else
    printf("DONE\n");
#endif

    // Numerical integration
    double angular_velocity = Calculus::trapezoidal_integral(a, nsamples);
#if REALTIME
    rt_printf("angular velocity = %f\n", angular_velocity);
#else
    printf("angular velocity = %f\n", angular_velocity);
#endif

    // Calculate kinetic energy
    rotational_energy = 0.5 * INERTIAL_MASS * angular_velocity * angular_velocity;
#if REALTIME
    rt_printf("rotational energy = %f\n", kinetic_energy);
#else
    printf("rotational energy = %f\n", kinetic_energy);
#endif
}

Behavior::type resolve_re_from_ang_acc() {
    Behavior::type behavior = (rotational_energy > MAX_RE) ?
        Behavior::type::Unsafe : Behavior::type::Safe;
    return behavior;
}

bool is_moving() {
    return (velocity->current == 0 || rose->velocity->current == 0) ? false : true;
}

bool arm_is_folded() {
    return rose->arm->current_position == 0 ? true : false;
}

bool arm_has_payload() {
    return rose->arm->has_payload;
}
