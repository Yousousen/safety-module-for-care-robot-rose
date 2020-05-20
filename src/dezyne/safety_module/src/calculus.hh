#include "constants.hh"

namespace Calculus {
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
