#ifndef CONSTANTS_HH
#define CONSTANTS_HH
#include <stdint.h>

// Read boundary values from CSV file.
#define CSV_HAS_POSITION 0
#define CSV_HAS_VELOCITY 0
#define CSV_HAS_ACCELERATION 0
#define CSV_HAS_ANGULAR_VELOCITY 0
#define CSV_HAS_ANGULAR_ACCELERATION 0

// Macros to control framebuffer device for led matrix.
#define DEV_INPUT_EVENT "/dev/input"
#define EVENT_DEV_NAME "event"
#define DEV_FB "/dev"
#define FB_DEV_NAME "fb"

// Framebuffer
struct fb_t {
	uint16_t pixel[8][8];
};

enum LedColor_t {BLUE = 0x0006, GREEN = 0x0300, RED = 0x3000};

enum ErrorCode_t { OK = 0x00, NOT_OK = 0x01 };
enum Behavior_t {UNSAFE = false, SAFE = true}; 


#endif // CONSTANTS_HH
