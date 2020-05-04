// Care robot representation
// In the file, retrieve functions update data from a sensor.
#ifndef CARE_ROBOT_HH
#define CARE_ROBOT_HH
#include <cstdlib>
#include <ctime>
#include <iostream>
#include <iomanip>
#include <string>
#include "physics.hh"
#include "constants.hh"

class GenericCareRobot {
    public:
        enum class MovementMode { STATIONARY };
        /* virtual ~GenericCareRobot(); */

        // Methods
        /* virtual Behavior_t retrieve_position(); */
        /* virtual Behavior_t retrieve_velocity(); */
        /* virtual Behavior_t retrieve_acceleration(); */
        /* virtual Behavior_t retrieve_angular_velocity(); */
        /* virtual Behavior_t retrieve_angular_acceleration(); */
        /* virtual Behavior_t retrieve_all(); */
};

// Components of a care robot.
class Body {
    public:
        // Constructors
        Body() : current_lift(0.0), change_lift(0.0), max_lift(0.0),
        min_lift(0.0) {};

        Body(double cur, double chg, double max, double min) : current_lift(cur),
        change_lift(chg), max_lift(max), min_lift(min) {};

        // Properties
        double current_lift;
        double change_lift;
        double max_lift;
        double min_lift;

        // Methods
        void retrieve_lift();

        friend std::ostream& operator<< (std::ostream&, const Body&);
};

class Neck {
    public:
        // Constructors
        Neck() : current_turn(0.0), change_turn(0.0), max_turn(0.0),
        min_turn(0.0) {};

        Neck(double cur, double chg, double max, double min) : current_turn(cur),
        change_turn(chg), max_turn(max), min_turn(min) {};

        // Properties
        double current_turn;
        double change_turn;
        double max_turn;
        double min_turn;

        // Methods
        void retrieve_turn();

        friend std::ostream& operator<< (std::ostream&, const Neck&);
};

class GripArm {
    public:
        // Constructors
        GripArm() : current_strength(0.0), change_strength(0.0),
        max_strength(0.0), min_strength(0.0), current_position(0.0),
        change_position(0.0), max_position(0.0), min_position(0.0),
        has_payload(false) {};

        GripArm(double curs, double chgs, double maxs, double mins, double
                curp, double chgp, double maxp, double minp, double pay) :
            current_strength(curs), change_strength(chgs), max_strength(maxs),
            min_strength(mins), current_position(curp), change_position(chgp),
            max_position(maxp), min_position(minp), has_payload(pay) {};


        // Properties
        double current_strength;
        double change_strength;
        double max_strength;
        double min_strength;
        double current_position;
        double change_position;
        double max_position;
        double min_position;

        // Conditions
        bool has_payload;

        // Methods
        void retrieve_strength();
        void retrieve_position();

        friend std::ostream& operator<< (std::ostream&, const GripArm&);
};

class Camera {
    public:
        // Constructors
        Camera() : object_nearby(false) {};
        Camera(double obn) : object_nearby(obn) {};

        // Conditions
        bool object_nearby;

        // Methods
        void retrieve_image();

        friend std::ostream& operator<< (std::ostream&, const Camera&);
};

class CareRobotRose : public GenericCareRobot {
    public:
        enum class MovementMode { STATIONARY, MOVING, TURNING };
        CareRobotRose();
        ~CareRobotRose();

        // Methods
        bool onMove();
        bool onTurn();
        bool onGrab();

        friend std::ostream& operator<< (std::ostream&, const CareRobotRose&);

        void retrieve_position();
        void retrieve_velocity();
        void retrieve_acceleration();
        void retrieve_angular_velocity();
        void retrieve_angular_acceleration();

        // Update all sensor values. Calls all retrieve functions of Rose
        // internally.
        void retrieve_all();

        MovementMode movement_mode;

        // Components and properties of care robot rose.
        Body* body = nullptr;
        Neck* neck = nullptr;
        GripArm* arm = nullptr;
        Camera* camera = nullptr;

        struct Position* position = nullptr;
        struct Velocity* velocity = nullptr;
        struct Acceleration* acceleration = nullptr;
        struct AngularVelocity* angular_velocity = nullptr;
        struct AngularAcceleration* angular_acceleration = nullptr;


    private:
};


#endif // CARE_ROBOT_HH
