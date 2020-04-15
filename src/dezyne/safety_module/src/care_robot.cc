#include "care_robot.hh"

CareRobotRose::CareRobotRose() {
    body = new Body(11.0, 12.0, 13.0, 14.0);
    neck = new Neck(11.0, 12.0, 13.0, 14.0);
    grip_arm = new GripArm(11.0, 12.0, 13.0, 14.0, 11.0, 12.0, 13.0, 14.0, true);
    camera = new Camera(true);

    movement_mode = MovementMode::STATIONARY;

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

CareRobotRose::~CareRobotRose() {
    if (body != nullptr)                 delete body;
    if (neck != nullptr)                 delete neck;
    if (grip_arm != nullptr)             delete grip_arm;
    if (camera != nullptr)               delete camera;
    if (position != nullptr)             delete position;
    if (velocity != nullptr)             delete velocity;
    if (acceleration != nullptr)         delete acceleration;
    if (angular_velocity != nullptr)     delete angular_velocity;
    if (angular_acceleration != nullptr) delete angular_acceleration;
}

Behavior_t Body::retrieve_lift() {
    Behavior_t isSafeBehavior = SAFE;
    auto previous_lift = this->current_lift;
    srand((unsigned)time(NULL));

    if ((this->current_lift = rand() % 100 + 1) > 50) {
        isSafeBehavior = UNSAFE;
    } else {
        isSafeBehavior = SAFE;
    }

    this->change_lift = this->current_lift - previous_lift;
    return isSafeBehavior;
}

Behavior_t Neck::retrieve_turn() {
    Behavior_t isSafeBehavior = SAFE;
    auto previous_turn = this->current_turn;
    srand((unsigned)time(NULL));

    if ((this->current_turn = rand() % 100 + 1) > 50) {
        isSafeBehavior = UNSAFE;
    } else {
        isSafeBehavior = SAFE;
    }

    this->change_turn = this->current_turn - previous_turn;
    return isSafeBehavior;
}

Behavior_t GripArm::retrieve_strength() {
    Behavior_t isSafeBehavior = SAFE;
    auto previous_strength = this->current_strength;
    srand((unsigned)time(NULL));

    if ((this->current_strength = rand() % 100 + 1) > 50) {
        isSafeBehavior = UNSAFE;
    } else {
        isSafeBehavior = SAFE;
    }
    
    this->change_strength = this->current_strength - previous_strength;
    return isSafeBehavior;
}

Behavior_t GripArm::retrieve_position() {
    Behavior_t isSafeBehavior = SAFE;
    auto previous_position = this->current_position;
    srand((unsigned)time(NULL));

    if ((this->current_position = rand() % 100 + 1) > 50) {
        isSafeBehavior = UNSAFE;
    } else {
        isSafeBehavior = SAFE;
    }

    this->change_position = this->current_position - previous_position;
    this->has_payload = static_cast<bool>(this->has_payload ^ 1);
    return isSafeBehavior;
}

Behavior_t Camera::retrieve_image() {
    this->object_nearby = static_cast<bool>(this->object_nearby ^ 1);
    return SAFE;
}

Behavior_t CareRobotRose::retrieve_position() {
    Behavior_t isSafeBehavior = SAFE;
    auto previous = this->position->current;
    srand((unsigned)time(NULL));

    if ((this->position->current = rand() % 100 + 1) > 50) {
        isSafeBehavior = UNSAFE;
    } else {
        isSafeBehavior = SAFE;
    }

    this->position->change = this->position->current - previous;
    return isSafeBehavior;
}

Behavior_t CareRobotRose::retrieve_velocity() {
    Behavior_t isSafeBehavior = SAFE;
    auto previous = this->velocity->current;
    srand((unsigned)time(NULL));

    if ((this->velocity->current = rand() % 100 + 1) > 50) {
        isSafeBehavior = UNSAFE;
    } else {
        isSafeBehavior = SAFE;
    }

    this->velocity->change = this->velocity->current - previous;
    return isSafeBehavior;;
}

Behavior_t CareRobotRose::retrieve_acceleration() {
    Behavior_t isSafeBehavior = SAFE;
    auto previous = this->acceleration->current;
    srand((unsigned)time(NULL));

    if ((this->acceleration->current = rand() % 100 + 1) > 50) {
        isSafeBehavior = UNSAFE;
    } else {
        isSafeBehavior = SAFE;
    }

    this->acceleration->change = this->acceleration->current - previous;
    return isSafeBehavior;
}

Behavior_t CareRobotRose::retrieve_angular_velocity() {
    Behavior_t isSafeBehavior = SAFE;
    auto previous = this->angular_velocity->current;
    srand((unsigned)time(NULL));

    if ((this->angular_velocity->current = rand() % 100 + 1) > 50) {
        isSafeBehavior = UNSAFE;
    } else {
        isSafeBehavior = SAFE;
    }

    this->angular_velocity->change = this->angular_velocity->current - previous;
    return isSafeBehavior;
}


Behavior_t CareRobotRose::retrieve_angular_acceleration() {
    Behavior_t isSafeBehavior = SAFE;
    auto previous = this->angular_acceleration->current;
    srand((unsigned)time(NULL));
    if ((this->angular_acceleration->current = rand() % 100 + 1)) {
        isSafeBehavior = UNSAFE;
    } else {
        isSafeBehavior = SAFE;
    }

    this->angular_acceleration->change = this->angular_acceleration->current -
        previous;
    return isSafeBehavior;
}

Behavior_t CareRobotRose::retrieve_all() {
    if (this->body->retrieve_lift() == UNSAFE) return UNSAFE;
    if (this->neck->retrieve_turn() == UNSAFE) return UNSAFE;
    if (this->grip_arm->retrieve_strength() == UNSAFE) return UNSAFE;
    if (this->grip_arm->retrieve_position() == UNSAFE) return UNSAFE;
    if (this->camera->retrieve_image() == UNSAFE) return UNSAFE;
    if (this->retrieve_position() == UNSAFE) return UNSAFE;
    if (this->retrieve_velocity() == UNSAFE) return UNSAFE;
    if (this->retrieve_acceleration() == UNSAFE) return UNSAFE;
    if (this->retrieve_angular_velocity() == UNSAFE) return UNSAFE;
    if (this->retrieve_angular_acceleration() == UNSAFE) return UNSAFE;
    return SAFE;
}

std::ostream& operator<< (std::ostream& os, const Body& body) {
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

std::ostream& operator<< (std::ostream& os, const Neck& neck) {
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

std::ostream& operator<< (std::ostream& os, const GripArm& grip_arm) {
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

std::ostream& operator<< (std::ostream& os, const Camera& camera) {
    os  << "### Camera ###:\n" << "object nearby: "  <<
        (camera.object_nearby ? "true" : "false") << "\n"
        << "\n";
    return os;
}

std::ostream& operator<< (std::ostream& os, const CareRobotRose& care_robot_rose) {
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
