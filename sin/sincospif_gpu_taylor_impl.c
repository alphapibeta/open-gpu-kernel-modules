#include "sincospif_gpu_impl.h"
#include <linux/types.h>
#include <linux/kernel.h>
#include <linux/math64.h>

#define M_PI 3.141592653589793
double sin_taylor_series(double x) {
    const double coefficients[] = {
        1.0,
        -1.0 / 6.0,
        1.0 / 120.0,
        -1.0 / 5040.0,
        1.0 / 362880.0,
        -1.0 / 39916800.0,
        1.0 / 6227020800.0
    };
    double result = 0.0;
    double term = x;
    double x_squared = x * x;

    for (int i = 0; i < 7; ++i) {
        result += coefficients[i] * term;
        term *= x_squared;
    }

    return result;
}

double cos_taylor_series(double x) {
    const double coefficients[] = {
        1.0,
        -1.0 / 2.0,
        1.0 / 24.0,
        -1.0 / 720.0,
        1.0 / 40320.0,
        -1.0 / 3628800.0,
        1.0 / 479001600.0
    };
    double result = 0.0;
    double term = 1.0;
    double x_squared = x * x;

    for (int i = 0; i < 7; ++i) {
        result += coefficients[i] * term;
        term *= x_squared;
    }

    return result;
}

double normalize_angle(double angle) {
    while (angle > M_PI) {
        angle -= 2.0 * M_PI;
    }
    while (angle < -M_PI) {
        angle += 2.0 * M_PI;
    }
    return angle;
}

void sincospif_gpu_calculate_taylor(float *angles, float *sin_results, float *cos_results, int num_angles) {
    int i;
    for (i = 0; i < num_angles; i++) {
        double normalized_angle = normalize_angle(angles[i] * M_PI);
        sin_results[i] = sin_taylor_series(normalized_angle);
        cos_results[i] = cos_taylor_series(normalized_angle);
    }
}
