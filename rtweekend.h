#ifndef RTWEEKEND_H
#define RTWEEKEND_H

#include <math.h>
#include <stdlib.h>   // rand, RAND_MAX

#define PI 3.1415926535897932385

static inline double degrees_to_radians(double degrees) {
    return degrees * PI / 180.0;
}

// Numero real aleatorio en [0, 1)
static inline double random_double(void) {
    return rand() / (RAND_MAX + 1.0);
}

// Numero real aleatorio en [min, max)
static inline double random_double_range(double min, double max) {
    return min + (max - min) * random_double();
}

// Recorta x al intervalo [min, max]
static inline double clamp(double x, double min, double max) {
    if (x < min) return min;
    if (x > max) return max;
    return x;
}

#endif
