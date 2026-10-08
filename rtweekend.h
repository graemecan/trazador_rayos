#ifndef RTWEEKEND_H
#define RTWEEKEND_H

#include <math.h>
#include <stdlib.h>   // rand, RAND_MAX
#include "vec3.h"

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

// Vector con componentes aleatorias
static inline vec3 vec3_random(void) {
    return v3(random_double(), random_double(), random_double());
}
static inline vec3 vec3_random_range(double min, double max) {
    return v3(random_double_range(min, max),
              random_double_range(min, max),
              random_double_range(min, max));
}

// Vector unitario con direccion uniformemente aleatoria
static inline vec3 random_unit_vector(void) {
    while (1) {
        vec3 p = vec3_random_range(-1, 1);
        double lensq = vec3_length_squared(p);
        if (1e-160 < lensq && lensq <= 1.0)   // dentro de la esfera y no degenerado
            return vec3_div(p, sqrt(lensq));
    }
}

// Vector aleatorio en el hemisferio de la normal (una alternativa)
static inline vec3 random_on_hemisphere(vec3 normal) {
    vec3 on_unit = random_unit_vector();
    return (vec3_dot(on_unit, normal) > 0.0) ? on_unit : vec3_neg(on_unit);
}

// ¿Es el vector practicamente cero? (lo usaremos en la Tarea 8)
static inline int vec3_near_zero(vec3 v) {
    double s = 1e-8;
    return (fabs(v.x) < s) && (fabs(v.y) < s) && (fabs(v.z) < s);
}

#endif
