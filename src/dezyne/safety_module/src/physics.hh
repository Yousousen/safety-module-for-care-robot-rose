// Here the position, velocity, acceleration, angular velocity, and angular
// acceleration structs can be found.
#ifndef PHYSICS_HH
#define PHYSICS_HH
#include <iostream>
#include <iomanip>

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

struct AngularDisplacement {
    // Constructors
    AngularDisplacement() : current(0.0), change(0.0), max(0.0), min(0.0) {};
    AngularDisplacement(double cur, double chg, double max, double min) :
        current(cur), change(chg), max(max), min(min) {};

    double current;
    double change;
    double max;
    double min;

    friend std::ostream& operator<< (std::ostream& os, const
            AngularDisplacement& angular_displacement) {
        os  << "### AngularDisplacement ###:\n"
            << "current: " << std::setw(25) << std::left  <<
            angular_displacement.current << "\n"
            << "change: " << std::setw(25) << std::left   <<
            angular_displacement.change << "\n"
            << "max: " << std::setw(25) << std::left      <<
            angular_displacement.max << "\n"
            << "min: " << std::setw(25) << std::left      <<
            angular_displacement.min << "\n"
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

#endif // PHYSICS_HH
