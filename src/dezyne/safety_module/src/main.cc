// Safety module

// The safety module creates a CareRobotRose object which serves as the safety
// module's interpretation of Rose, i.e.  what the safety module needs from
// Rose.

#include "safety_module.hh"

auto main() -> int {
    // Start the safety module.
    SafetyModule* sm = new SafetyModule();
    sm->roll();
    delete sm;
}
