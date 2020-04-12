// Safety module
// The module contains a pointer to a care robot, which provides the module
// with a representation of a care robot that is useful to it.  In the file,
// retrieve functions update data from a sensor.
#ifndef SAFETY_MODULE_HH
#define SAFETY_MODULE_HH
#include <cstdlib>
#include <ctime>
#include <iostream>
#include <iomanip>
#include <string>
#include "constants.hh"
#include "care_robot.hh"
#include "physics.hh"

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



class SafetyModule {
    public:
        SafetyModule();
        ~SafetyModule();

        // Methods
        // Run the safety module
        ErrorCode_t roll();

        // Light the led matrix in the specified color.
        ErrorCode_t light_led(struct fb_t* fb, LedColor_t color);

        Behavior_t retrieve_position();
        Behavior_t retrieve_velocity();
        Behavior_t retrieve_acceleration();
        Behavior_t retrieve_angular_velocity();
        Behavior_t retrieve_angular_acceleration();

        // Update all sensor values. Calls all retrieve functions of Rose
        // internally.
        Behavior_t retrieve_all();

        struct Position* position = nullptr;
        struct Velocity* velocity = nullptr;
        struct Acceleration* acceleration = nullptr;
        struct AngularVelocity* angular_velocity = nullptr;
        struct AngularAcceleration* angular_acceleration = nullptr;

        friend std::ostream& operator<< (std::ostream&, const SafetyModule&);

        // Properties
        CareRobotRose* rose = nullptr;
        /* GenericCareRobot* rose = nullptr; */

    private:
};



#endif // SAFETY_MODULE_HH
