#include <iostream>

int main() {
    const int inertial_mass = 100;
    const int  side = 100;
    std::cout << "((1/6) * inertial_mass) = "  << ((1.0/6) * inertial_mass) << "\n";
    std::cout << "((1/6) * inertial_mass * side * side) = "  << ((1/6) *
            inertial_mass * side * side) << "\n";
}
