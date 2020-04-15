// Safety module

// The safety module, which is the entire system creates a CareRobotRose rose
// which serves as the safety module's interpretation of Rose, i.e.  what he
// needs from Rose.

#include <iostream>
#include "safety_module.hh"

auto main() -> int {
    SafetyModule* sm = new SafetyModule();
    sm->roll();
    delete sm;
    return 0;
}
