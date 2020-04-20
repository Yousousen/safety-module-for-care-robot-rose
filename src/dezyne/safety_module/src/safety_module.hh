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
#include "physics.hh"
#include "care_robot.hh"




class SafetyModule {
    public:
        SafetyModule();
        ~SafetyModule();

        // Methods
        // Run the safety module
        ErrorCode_t roll();

        // Construct and destruct the frame buffer
        static void initialise_framebuffer();
        static void destruct_framebuffer();
        // Light the led matrix in the specified color.
        static void light_led(struct fb_t* fb, unsigned color);

        void retrieve_position();
        void retrieve_velocity();
        void retrieve_acceleration();
        void retrieve_angular_velocity();
        void retrieve_angular_acceleration();

        // Sample methods
        void sample_acceleration(double* f, const int nsamples);

        // Check methods
        Behavior_t check_acceleration();

        // Update all sensor values. Calls all retrieve functions of Rose
        // internally.
        void retrieve_all();

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
