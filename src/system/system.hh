// Care robot representation
// In the file, retrieve functions update data from a sensor.


#ifndef CAREROBOT_H
#define CAREROBOT_H
#include <cstdlib>
#include <ctime>
#include <iostream>
#include <iomanip>
#include <string>

#define CSV_HAS_POSITION 0
#define CSV_HAS_VELOCITY 0
#define CSV_HAS_ACCELERATION 0
#define CSV_HAS_ANGULAR_VELOCITY 0
#define CSV_HAS_ANGULAR_ACCELERATION 0

enum ErrorCodes { OK = 0x00, NOT_OK = 0x01 };

class GenericCareRobot {
    public:
        enum class MovementMode { STATIONARY };
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
        int retrieve_lift();

        friend std::ostream& operator<< (std::ostream& os, const Body& body) {
            os << "### Body ###:\n"
                << "current lift: " << std::setw(25) << std::left <<
                body.current_lift << "\n"
                << "change_lift: " << std::setw(25) << std::left <<
                body.change_lift << "\n"
                << "max_lift: " << std::setw(25) << std::left << body.max_lift
                << "\n"
                << "min_lift: " << std::setw(25) << std::left << body.min_lift
                << "\n"
                << "\n";
            return os;
        }
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

        friend std::ostream& operator<< (std::ostream& os, const Neck& neck) {
            os << "### Neck ###:\n"
                << "current turn: " << std::setw(25) << std::left <<
                neck.current_turn << "\n"
                << "change_turn: " << std::setw(25) << std::left <<
                neck.change_turn << "\n"
                << "max_turn: " << std::setw(25) << std::left << neck.max_turn
                << "\n"
                << "min_turn: " << std::setw(25) << std::left << neck.min_turn
                << "\n"
                << "\n";
            return os;
        }
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

        friend std::ostream& operator<< (std::ostream& os, const GripArm& grip_arm) {
            os  << "### GripArm ###:\n"
                << "current strength: "  << grip_arm.current_strength << "\n"
                << "change_strength: "   << grip_arm.change_strength << "\n"
                << "max_strength: "      << grip_arm.max_strength << "\n"
                << "min_strength: "      << grip_arm.min_strength << "\n"
                << "current position: "  << grip_arm.current_position << "\n"
                << "change_position: "   << grip_arm.change_position << "\n"
                << "max_position: "      << grip_arm.max_position << "\n"
                << "min_position: "      << grip_arm.min_position << "\n"
                << "has_payload: "      << (grip_arm.has_payload ? "true" :
                        "false") << "\n"
                << "\n";
            return os;
        }
};

class Camera {
    public:
        // Constructors
        Camera() : object_nearby(false) {};
        Camera(double obn) : object_nearby(obn) {};

        // Conditions
        bool object_nearby;

        friend std::ostream& operator<< (std::ostream& os, const Camera& camera) {
            os  << "### Camera ###:\n" << "object nearby: "  <<
                (camera.object_nearby ? "true" : "false") << "\n"
                << "\n";
            return os;
        }
};

class CareRobotRose : public GenericCareRobot {
    public:
        enum class MovementMode { STATIONARY, MOVING, TURNING };

        struct Position {
            // Constructors
            Position() : current(0.0), change(0.0), max(0.0), min(0.0) {};
            Position(double cur, double chg, double max, double min) :
                current(cur), change(chg), max(max), min(min) {};

            double current;
            double change;
            double max;
            double min;

            friend std::ostream& operator<< (std::ostream& os, const Position&
                    position) {
                os  << "### Position ###:\n"
                    << "current: " << std::setw(25) << std::left  <<
                    position.current << "\n"
                    << "change: " << std::setw(25) << std::left   <<
                    position.change << "\n"
                    << "max: " << std::setw(25) << std::left      <<
                    position.max << "\n"
                    << "min: " << std::setw(25) << std::left      <<
                    position.min << "\n"
                    << "\n";
                return os;
            }
        };

        struct Velocity {
            // Constructors
            Velocity() : current(0.0), change(0.0), max(0.0), min(0.0) {};
            Velocity(double cur, double chg, double max, double min) :
                current(cur), change(chg), max(max), min(min) {};

            double current;
            double change;
            double max;
            double min;

            friend std::ostream& operator<< (std::ostream& os, const Velocity&
                    velocity) {
                os  << "### Velocity ###:\n"
                    << "current: " << std::setw(25) << std::left  <<
                    velocity.current << "\n"
                    << "change: " << std::setw(25) << std::left   <<
                    velocity.change << "\n"
                    << "max: " << std::setw(25) << std::left      <<
                    velocity.max << "\n"
                    << "min: " << std::setw(25) << std::left      <<
                    velocity.min << "\n"
                    << "\n";
                return os;
            }
        };

        struct Acceleration {
            // Constructors
            Acceleration() : current(0.0), change(0.0), max(0.0), min(0.0) {};
            Acceleration(double cur, double chg, double max, double min) :
                current(cur), change(chg), max(max), min(min) {};

            double current;
            double change;
            double max;
            double min;

            friend std::ostream& operator<< (std::ostream& os, const Acceleration&
                    acceleration) {
                os  << "### Acceleration ###:\n"
                    << "current: " << std::setw(25) << std::left  <<
                    acceleration.current << "\n"
                    << "change: " << std::setw(25) << std::left   <<
                    acceleration.change << "\n"
                    << "max: " << std::setw(25) << std::left      <<
                    acceleration.max << "\n"
                    << "min: " << std::setw(25) << std::left      <<
                    acceleration.min << "\n"
                    << "\n";
                return os;
            }
        };

        struct AngularVelocity {
            // Constructors
            AngularVelocity() : current(0.0), change(0.0), max(0.0), min(0.0) {};
            AngularVelocity(double cur, double chg, double max, double min) :
                current(cur), change(chg), max(max), min(min) {};

            double current;
            double change;
            double max;
            double min;

            friend std::ostream& operator<< (std::ostream& os, const
                    AngularVelocity& angular_velocity) {
                os  << "### AngularVelocity ###:\n"
                    << "current: " << std::setw(25) << std::left  <<
                    angular_velocity.current << "\n"
                    << "change: " << std::setw(25) << std::left   <<
                    angular_velocity.change << "\n"
                    << "max: " << std::setw(25) << std::left      <<
                    angular_velocity.max << "\n"
                    << "min: " << std::setw(25) << std::left      <<
                    angular_velocity.min << "\n"
                    << "\n";
                return os;
            }
        };

        struct AngularAcceleration {
            // Constructors
            AngularAcceleration() : current(0.0), change(0.0), max(0.0), min(0.0) {};
            AngularAcceleration(double cur, double chg, double max, double min)
                : current(cur), change(chg), max(max), min(min) {};

            double current;
            double change;
            double max;
            double min;

            friend std::ostream& operator<< (std::ostream& os, const
                    AngularAcceleration& angular_acceleration) {
                os  << "### AngularAcceleration ###:\n"
                    << "current: " << std::setw(25) << std::left  <<
                    angular_acceleration.current << "\n"
                    << "change: " << std::setw(25) << std::left   <<
                    angular_acceleration.change << "\n"
                    << "max: " << std::setw(25) << std::left      <<
                    angular_acceleration.max << "\n"
                    << "min: " << std::setw(25) << std::left      <<
                    angular_acceleration.min << "\n"
                    << "\n";
                return os;
            }
        };

        CareRobotRose();
        ~CareRobotRose();

        // Methods
        int onMove();
        int onTurn();
        int onGrab();

        MovementMode movement_mode;

        // Components and properties of care robot rose.
        Body* body = nullptr;
        Neck* neck = nullptr;
        GripArm* grip_arm = nullptr;
        Camera* camera = nullptr;

        struct Position* position = nullptr;
        struct Velocity* velocity = nullptr;
        struct Acceleration* acceleration = nullptr;
        struct AngularVelocity* angular_velocity = nullptr;
        struct AngularAcceleration* angular_acceleration = nullptr;


        friend std::ostream& operator<< (std::ostream& os, const CareRobotRose&
                care_robot_rose) {
            os  << "##### Rose #####:\n\n"
                << *care_robot_rose.body
                << *care_robot_rose.neck
                << *care_robot_rose.grip_arm
                << *care_robot_rose.camera
                << *care_robot_rose.position
                << *care_robot_rose.velocity
                << *care_robot_rose.acceleration
                << *care_robot_rose.angular_velocity
                << *care_robot_rose.angular_acceleration
                ;

            return os;
        }
    private:
};


#endif // CAREROBOT_H
