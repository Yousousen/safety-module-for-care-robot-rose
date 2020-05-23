#ifndef CONSTANTS_HH
#define CONSTANTS_HH
#include <stdint.h>
#include <pthread.h>
#include "System.hh"


// TODO: Remove the notions of CSV.
// Read boundary values from CSV file.
#define CSV_HAS_POSITION 0
#define CSV_HAS_VELOCITY 0
#define CSV_HAS_ACCELERATION 0
#define CSV_HAS_ANGULAR_VELOCITY 0
#define CSV_HAS_ANGULAR_ACCELERATION 0

// Interial mass of the care robot with safety module attached (kg).
#define INERTIAL_MASS 100
// Approximation of robot side length
#define RADIUS 1
// Approximate moment of inertia like robot is a rectangular block, rotating
// around its center of mass (kg m^2).
#define MOMENT_OF_INERTIA (0.5 * INERTIAL_MASS * RADIUS * RADIUS)

// For sampling and calculus
// Δt (ms)
/* #define CHANGE_IN_TIME_MS 1 */
// Δt (μs)
#define CHANGE_IN_TIME_MICRO 100
// Δt (s)
/* #define CHANGE_IN_TIME (CHANGE_IN_TIME_MS * 1E-3) */
#define CHANGE_IN_TIME (CHANGE_IN_TIME_MICRO * 1E-6)

// Macros to control framebuffer device for led matrix.
#define DEV_INPUT_EVENT "/dev/input"
#define EVENT_DEV_NAME "event"
#define DEV_FB "/dev"
#define FB_DEV_NAME "fb"

#define XDDP_PORT_LIGHT_LED 0
#define XDDP_PORT_RESET_LED 1
#define XDDP_PORT_RET_ACC 2
#define XDDP_PORT_RET_ANG_DISP 3
#define XDDP_PORT_RET_FORCE 4
#define XDDP_PORT_RET_TORQUE 5
#define XDDP_PORT_RET_POS 6

#define NSEC_PER_SEC 1000000000ULL

#define SIZE 10
#define BUFSIZE 128

#define PERIODIC_CHECKS 1

/*** Structs ***/
// Framebuffer
struct fb_t {
	uint16_t pixel[8][8];
};

// TODO: Is this any better than a global variable?
// Remember that if I change this I should take into account the fact that if I
// pass a pointer in a function I do not need &, and if I don't I need & for
// things like sem_wait.
// Arguments for threads
// Not everything in the struct is needed by every thread, the thread just
// takes what it needs.
struct threadargs {
    // Dezyne system
    System* s;
    int xddp_socket;
};

// Periodic threads information.
struct th_info {
    int period;
    void (*body)(void* args);
    // Dezyne system
    System* s;
};

struct periodic_task {
  struct timespec ts;
  int period;
};
struct periodic_task;


/*** enums ***/
enum LedColor_t {BLUE = 0x0006, GREEN = 0x0300, RED = 0x3000};
enum ErrorCode_t { OK = 0x00, NOT_OK = 0x01 };
enum Behavior_t {UNSAFE = false, SAFE = true}; 


#endif // CONSTANTS_HH
