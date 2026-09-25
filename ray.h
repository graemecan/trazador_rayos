#ifndef RAY_H
#define RAY_H

#include "vec3.h"

// Un rayo P(t) = origen + t * direccion
typedef struct {
    point3 orig;   // A: de donde parte
    vec3   dir;    // b: hacia donde apunta
} ray;

// Punto del rayo para el parametro t
static inline point3 ray_at(ray r, double t) {
    return vec3_add(r.orig, vec3_scale(r.dir, t));
}

#endif
