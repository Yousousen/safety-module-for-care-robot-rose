#include <cstdio>
#include <ctime>
#include <cstdlib>
#include <unistd.h>
#include <thread>
#include <chrono>

// Δt (ms)
/* #define CHANGE_IN_TIME_MS 1 */
// Δt (μs)
#define CHANGE_IN_TIME_MICRO 1000
// Δt (s)
/* #define CHANGE_IN_TIME (CHANGE_IN_TIME_MS * 1E-3) */
#define CHANGE_IN_TIME (CHANGE_IN_TIME_MICRO * 1E-6)

#define INERTIAL_MASS 100
#define MAX_KINETIC_ENERGY 400

namespace Math {
    double trapezoidal_integral(double* f, int nsamples) {
        // Using trapezoid rule.
        // integral = h/2 * ( y0 + 2y1 + 2y2 + 2y3 ....... + yn)
        int sum = 0;
        sum += f[0];
        int i;
        for (i = 1; i < nsamples-1; ++i) {
            sum += 2*f[i];
        }
        sum += f[i];
        return (double) ((CHANGE_IN_TIME/2) * sum);
    }
}

double retrieve_acceleration() {
    srand((unsigned)time(NULL));
    return rand() % 5 + 1;
    /* return 2; */
}


void sample(double* f, const int nsamples, double (*retrieve_func)()) {
    printf("# rectangles: ");
    int i;
    for (i = 0; i < nsamples; ++i) {
        f[i] = retrieve_func();
        /* std::this_thread::sleep_for(std::chrono::milliseconds(CHANGE_IN_TIME_MS)); */
        std::this_thread::sleep_for(std::chrono::microseconds(CHANGE_IN_TIME_MICRO));
    }
    printf("%d, ", i);
}

auto main() -> int {
    // numbers of seconds to sample.
    const double nseconds = 0.5;
    const int nsamples = (int) (nseconds / CHANGE_IN_TIME);
    printf("nsamples: %d\n", nsamples);

    // Update acceleration
    double a[nsamples];
    // Sample acceleration for CHANGE_IN_TIME long.

    printf("SAMPLING ACCELERATION\n");
    sample(a, nsamples, retrieve_acceleration);
    printf("DONE\n");

    double velocity = Math::trapezoidal_integral(a, nsamples);
    printf("velocity = %f\n", velocity);

    // Calculate kinetic energy
    const double kinetic_energy = 0.5 * INERTIAL_MASS * velocity * velocity;
    printf("Kinetic energy = %g\n", kinetic_energy);
    if (kinetic_energy >= MAX_KINETIC_ENERGY) {
        printf("TOO MANY JOULES\n");
    } else {
        printf("ALL FINE");
    }

    return 0;
}
