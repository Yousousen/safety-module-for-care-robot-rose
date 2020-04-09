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

class SafetyModule {
    public:
        SafetyModule();
        ~SafetyModule();

        // Methods
        // Run the safety module
        ErrorCode roll();

        ErrorCode retrieve_position();
        ErrorCode retrieve_velocity();
        ErrorCode retrieve_acceleration();
        ErrorCode retrieve_angular_velocity();
        ErrorCode retrieve_angular_acceleration();

        // Update all sensor values. Calls all retrieve functions of Rose
        // internally.
        ErrorCode retrieve_all();

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
