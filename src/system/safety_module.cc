#include "safety_module.hh"

SafetyModule::SafetyModule() {
    // Create a rose representation
    this->rose = new CareRobotRose();

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

SafetyModule::~SafetyModule() {
    if (rose != nullptr)                 delete rose;
    if (position != nullptr)             delete position;
    if (velocity != nullptr)             delete velocity;
    if (acceleration != nullptr)         delete acceleration;
    if (angular_velocity != nullptr)     delete angular_velocity;
    if (angular_acceleration != nullptr) delete angular_acceleration;
}

ErrorCode SafetyModule::roll() {
    std::cout << *this->rose << "\n";
    this->rose->retrieve_all();
    std::cout << *rose << "\n";
    this->rose->retrieve_all();
    std::cout << *rose << "\n";
    return OK;
}


ErrorCode SafetyModule::retrieve_position() {
    auto previous = this->position->current;
    srand((unsigned)time(NULL));
    this->position->current = rand() % 100 + 1;
    this->position->change = this->position->current - previous;
    return OK;
}

ErrorCode SafetyModule::retrieve_velocity() {
    auto previous = this->velocity->current;
    srand((unsigned)time(NULL));
    this->velocity->current = rand() % 100 + 1;
    this->velocity->change = this->velocity->current - previous;
    return OK;
}

ErrorCode SafetyModule::retrieve_acceleration() {
    auto previous = this->acceleration->current;
    srand((unsigned)time(NULL));
    this->acceleration->current = rand() % 100 + 1;
    this->acceleration->change = this->acceleration->current - previous;
    return OK;
}

ErrorCode SafetyModule::retrieve_angular_velocity() {
    auto previous = this->angular_velocity->current;
    srand((unsigned)time(NULL));
    this->angular_velocity->current = rand() % 100 + 1;
    this->angular_velocity->change = this->angular_velocity->current - previous;
    return OK;
}


ErrorCode SafetyModule::retrieve_angular_acceleration() {
    auto previous = this->angular_acceleration->current;
    srand((unsigned)time(NULL));
    this->angular_acceleration->current = rand() % 100 + 1;
    this->angular_acceleration->change = this->angular_acceleration->current -
        previous;
    return OK;
}

ErrorCode SafetyModule::retrieve_all() {
    this->retrieve_position();
    this->retrieve_velocity();
    this->retrieve_acceleration();
    this->retrieve_angular_velocity();
    this->retrieve_angular_acceleration();
    return OK;
}

std::ostream& operator<< (std::ostream& os, const SafetyModule&
        safety_module) {
    os  << "##### Safety Module #####:\n\n"
        << *safety_module.position
        << *safety_module.velocity
        << *safety_module.acceleration
        << *safety_module.angular_velocity
        << *safety_module.angular_acceleration
        ;

    return os;
}

