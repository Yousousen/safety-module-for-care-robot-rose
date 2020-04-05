#include "system.hh"

int Body::retrieve_lift() {
    auto old_lift = current_lift;
    srand((unsigned)time(NULL));
    current_lift = rand() % 100 + 1;
    change_lift = current_lift - old_lift;
    return OK;
}

CareRobotRose::CareRobotRose()
{
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
