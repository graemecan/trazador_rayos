#ifndef VEC3_H
#define VEC3_H

#include <math.h>   // sqrt

// Un vector de 3 componentes reales. Lo reutilizamos para puntos,
// direcciones y colores: por dentro son los mismos tres numeros.
typedef struct {
    double x, y, z;
} vec3;

// Alias que dan significado al codigo (son EL MISMO tipo por dentro).
typedef vec3 point3;   // un punto en el espacio 3D
typedef vec3 color;    // un color RGB, cada componente en [0,1]

// Constructor comodo: v3(x,y,z) crea un vec3.
static inline vec3 v3(double x, double y, double z) {
    vec3 v = { x, y, z };
    return v;
}

// Operaciones basicas (sustituyen a los operadores de C++)
static inline vec3 vec3_add(vec3 a, vec3 b)   { return v3(a.x+b.x, a.y+b.y, a.z+b.z); }
static inline vec3 vec3_sub(vec3 a, vec3 b)   { return v3(a.x-b.x, a.y-b.y, a.z-b.z); }
static inline vec3 vec3_mul(vec3 a, vec3 b)   { return v3(a.x*b.x, a.y*b.y, a.z*b.z); } // componente a componente
static inline vec3 vec3_scale(vec3 a, double t){ return v3(a.x*t, a.y*t, a.z*t); }
static inline vec3 vec3_div(vec3 a, double t) { return vec3_scale(a, 1.0/t); }
static inline vec3 vec3_neg(vec3 a)           { return v3(-a.x, -a.y, -a.z); }

// Producto escalar y producto vectorial
static inline double vec3_dot(vec3 a, vec3 b) { return a.x*b.x + a.y*b.y + a.z*b.z; }
static inline vec3   vec3_cross(vec3 a, vec3 b) {
    return v3(a.y*b.z - a.z*b.y,
              a.z*b.x - a.x*b.z,
              a.x*b.y - a.y*b.x);
}

// Longitud (norma) y normalizacion
static inline double vec3_length_squared(vec3 a) { return vec3_dot(a, a); }
static inline double vec3_length(vec3 a)         { return sqrt(vec3_length_squared(a)); }
static inline vec3   vec3_unit(vec3 a)           { return vec3_div(a, vec3_length(a)); }

#endif
