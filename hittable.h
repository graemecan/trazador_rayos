#ifndef HITTABLE_H
#define HITTABLE_H

#include "ray.h"

// Declaracion adelantada: "material" se define en material.h (Tarea 8).
// Aqui solo necesitamos poder guardar un puntero a el.
struct material;

// Datos de un impacto rayo-superficie.
typedef struct {
    point3 p;                    // punto de impacto
    vec3   normal;               // normal (unitaria) en p
    double t;                    // parametro del rayo en el impacto
    int    front_face;           // 1 si el rayo llega por fuera de la superficie
    const struct material *mat;  // material del objeto golpeado (Tarea 8)
} hit_record;

// Ajusta la normal para que siempre apunte "contra" el rayo, y recuerda
// si golpeamos la cara exterior o interior.
static inline void set_face_normal(hit_record *rec, ray r, vec3 outward_normal) {
    rec->front_face = vec3_dot(r.dir, outward_normal) < 0;
    rec->normal = rec->front_face ? outward_normal : vec3_neg(outward_normal);
}

// Una esfera: centro, radio y (mas adelante) su material.
typedef struct {
    point3 center;
    double radius;
    const struct material *mat;
} sphere;

// ¿El rayo golpea la esfera dentro del intervalo (ray_tmin, ray_tmax)?
// Si es asi, rellena *rec y devuelve 1.
static int sphere_hit(sphere s, ray r, double ray_tmin, double ray_tmax, hit_record *rec) {
    vec3 oc = vec3_sub(s.center, r.orig);
    double a = vec3_length_squared(r.dir);
    double h = vec3_dot(r.dir, oc);
    double c = vec3_length_squared(oc) - s.radius * s.radius;

    double discriminant = h*h - a*c;
    if (discriminant < 0) return 0;
    double sqrtd = sqrt(discriminant);

    // Buscamos la raiz mas cercana dentro del rango valido.
    double root = (h - sqrtd) / a;
    if (root <= ray_tmin || root >= ray_tmax) {
        root = (h + sqrtd) / a;
        if (root <= ray_tmin || root >= ray_tmax) return 0;
    }

    rec->t = root;
    rec->p = ray_at(r, root);
    vec3 outward_normal = vec3_div(vec3_sub(rec->p, s.center), s.radius);
    set_face_normal(rec, r, outward_normal);
    rec->mat = s.mat;
    return 1;
}

// El "mundo": un array de esferas y su cantidad.
typedef struct {
    sphere *spheres;   // puntero al array de esferas
    int     count;     // cuantas hay
} hittable_list;

// Golpea el objeto MAS CERCANO de toda la lista.
static int world_hit(hittable_list world, ray r,
                     double ray_tmin, double ray_tmax, hit_record *rec) {
    hit_record temp;
    int hit_anything = 0;
    double closest = ray_tmax;   // vamos acortando el rango al objeto mas cercano

    for (int i = 0; i < world.count; i++) {
        if (sphere_hit(world.spheres[i], r, ray_tmin, closest, &temp)) {
            hit_anything = 1;
            closest = temp.t;    // ahora solo aceptamos impactos aun mas cercanos
            *rec = temp;         // copiamos el mejor impacto hasta ahora
        }
    }
    return hit_anything;
}

#endif
