// Safety module

// The safety module, which is the entire system, or the main function, creates
// a CareRobotRose rose which serves as the safety module's interpretation of
// Rose, i.e.  what he needs from Rose.

#include <iostream>
#include "system.hh"

int main() {
    CareRobotRose* rose = new CareRobotRose();
    rose->body->retrieve_lift();
    std::cout << *rose->body << "\n";

    delete rose;
    return 0;
}
