#include "care_robot.hh"

CareRobotRose::CareRobotRose() {
    body = new Body(0.0, 0.0, 0.0, 0.0);
    neck = new Neck(0.0, 0.0, 0.0, 0.0);
    arm = new Arm(0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, true);
    camera = new Camera(true);

    position = new Position(0.0, 0.0, 0.0, 0.0);
    velocity = new Velocity(0.0, 0.0, 0.0, 0.0);
    acceleration = new Acceleration(0.0, 0.0, 0.0, 0.0);
    angular_acceleration = new AngularAcceleration(0.0, 0.0, 0.0, 0.0);
    angular_velocity = new AngularVelocity(0.0, 0.0, 0.0, 0.0);
    angular_acceleration = new AngularAcceleration(0.0, 0.0, 0.0, 0.0);

    movement_mode = MovementMode::STATIONARY;
    is_moving = false;
}

CareRobotRose::~CareRobotRose() {
    if (body != nullptr)                 delete body;
    if (neck != nullptr)                 delete neck;
    if (arm != nullptr)             delete arm;
    if (camera != nullptr)               delete camera;
    if (position != nullptr)             delete position;
    if (velocity != nullptr)             delete velocity;
    if (acceleration != nullptr)         delete acceleration;
    if (angular_velocity != nullptr)     delete angular_velocity;
    if (angular_acceleration != nullptr) delete angular_acceleration;
}

void Body::retrieve_lift() {
    auto previous_lift = this->current_lift;
    srand((unsigned)time(NULL));
    this->current_lift = rand() % 100 + 1;
    this->change_lift = this->current_lift - previous_lift;
}

void Neck::retrieve_turn() {
    auto previous_turn = this->current_turn;
    srand((unsigned)time(NULL));
    this->current_turn = rand() % 100 + 1;
    this->change_turn = this->current_turn - previous_turn;
}

void Arm::retrieve_force() {
    auto previous_force = this->current_force;
    srand((unsigned)time(NULL));
    this->current_force = rand() % 100 + 1;
    this->change_force = this->current_force - previous_force;
    this->has_payload = static_cast<bool>(this->has_payload ^ 1);
}

void Arm::retrieve_torque() {
    auto previous_torque = this->current_torque;
    srand((unsigned)time(NULL));
    this->current_torque = rand() % 100 + 1;
    this->change_torque = this->current_torque - previous_torque;
    this->has_payload = static_cast<bool>(this->has_payload ^ 1);
}

void Arm::retrieve_position() {
    auto previous_position = this->current_position;
    srand((unsigned)time(NULL));
    /* this->current_position = rand() % 100 + 1; */
    this->current_position = rand() % 2;
    this->change_position = this->current_position - previous_position;
}

void Camera::retrieve_image() {
    this->object_nearby = static_cast<bool>(this->object_nearby ^ 1);
}

void CareRobotRose::retrieve_position() {
    auto previous = this->position->current;
    srand((unsigned)time(NULL));
    this->position->current = rand() % 100 + 1;
    this->position->change = this->position->current - previous;
}

void CareRobotRose::retrieve_velocity() {
    auto previous = this->velocity->current;
    srand((unsigned)time(NULL));
    this->velocity->current = rand() % 100 + 1;
    this->velocity->change = this->velocity->current - previous;
}

void CareRobotRose::retrieve_acceleration() {
    auto previous = this->acceleration->current;
    srand((unsigned)time(NULL));
    this->acceleration->current = rand() % 100 + 1;
    this->acceleration->change = this->acceleration->current - previous;
}

void CareRobotRose::retrieve_angular_velocity() {
    auto previous = this->angular_velocity->current;
    srand((unsigned)time(NULL));
    this->angular_velocity->current = rand() % 100 + 1;
    this->angular_velocity->change = this->angular_velocity->current - previous;
}


void CareRobotRose::retrieve_angular_acceleration() {
    auto previous = this->angular_acceleration->current;
    srand((unsigned)time(NULL));
    this->angular_acceleration->current = rand() % 100 + 1;
    this->angular_acceleration->change = this->angular_acceleration->current -
        previous;
}

void CareRobotRose::retrieve_all() {
    this->body->retrieve_lift();
    this->neck->retrieve_turn();
    this->arm->retrieve_force();
    this->arm->retrieve_torque();
    this->arm->retrieve_position();
    this->camera->retrieve_image();
    this->retrieve_position();
    this->retrieve_velocity();
    this->retrieve_acceleration();
    this->retrieve_angular_velocity();
    this->retrieve_angular_acceleration();
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

std::ostream& operator<< (std::ostream& os, const Arm& arm) {
    os  << "### Arm ###:\n"
        << "current force: "  << arm.current_force << "\n"
        << "change_force: "   << arm.change_force << "\n"
        << "max_force: "      << arm.max_force << "\n"
        << "min_force: "      << arm.min_force << "\n"
        << "current torque: "  << arm.current_torque << "\n"
        << "change_torque: "   << arm.change_torque << "\n"
        << "max_torque: "      << arm.max_torque << "\n"
        << "min_torque: "      << arm.min_torque << "\n"
        << "current position: "  << arm.current_position << "\n"
        << "change_position: "   << arm.change_position << "\n"
        << "max_position: "      << arm.max_position << "\n"
        << "min_position: "      << arm.min_position << "\n"
        << "has_payload: "      << (arm.has_payload ? "true" :
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
        << *care_robot_rose.arm
        << *care_robot_rose.camera
        << *care_robot_rose.position
        << *care_robot_rose.velocity
        << *care_robot_rose.acceleration
        << *care_robot_rose.angular_velocity
        << *care_robot_rose.angular_acceleration
        ;

    return os;
}
