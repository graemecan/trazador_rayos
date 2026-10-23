#ifndef MATERIAL_H
#define MATERIAL_H

#include "rtweekend.h"
#include "hittable.h"

// La "etiqueta": que clase de material es.
typedef enum {
    LAMBERTIAN,   // mate
    METAL,        // metalico
    DIELECTRIC    // vidrio (Tarea 9)
} material_type;

// Un unico struct que sirve para los tres materiales.
// Cada variante usa solo los campos que le interesan.
typedef struct material {
    material_type type;
    color  albedo;             // LAMBERTIAN y METAL: color/atenuacion
    double fuzz;               // METAL: cuanto se difumina el reflejo (0 = espejo)
    double refraction_index;   // DIELECTRIC: indice de refraccion
} material;

// Aproximacion de Schlick: fraccion reflejada segun el angulo.
static inline double reflectance(double cosine, double refraction_index) {
    double r0 = (1 - refraction_index) / (1 + refraction_index);
    r0 = r0 * r0;
    return r0 + (1 - r0) * pow(1 - cosine, 5);
}

// scatter: dado un rayo entrante y un impacto, decide el rayo dispersado
// y cuanto atenua el color. Devuelve 0 si el rayo se absorbe.
static int scatter(const material *mat, ray r_in, hit_record rec,
                   color *attenuation, ray *scattered) {
    switch (mat->type) {

    case LAMBERTIAN: {
        // Rebote difuso: normal + direccion aleatoria
        vec3 dir = vec3_add(rec.normal, random_unit_vector());
        if (vec3_near_zero(dir)) dir = rec.normal;   // evita direccion nula
        scattered->orig = rec.p;
        scattered->dir  = dir;
        *attenuation = mat->albedo;
        return 1;
    }

    case METAL: {
        // Reflejo especular, opcionalmente difuminado por "fuzz"
        vec3 reflected = reflect(r_in.dir, rec.normal);
        reflected = vec3_add(vec3_unit(reflected),
                             vec3_scale(random_unit_vector(), mat->fuzz));
        scattered->orig = rec.p;
        scattered->dir  = reflected;
        *attenuation = mat->albedo;
        // Si el rayo difuminado se hunde bajo la superficie, se absorbe.
        return vec3_dot(scattered->dir, rec.normal) > 0;
    }

    // Dentro de scatter(), sustituye el "case DIELECTRIC" por:
    case DIELECTRIC: {
	*attenuation = v3(1.0, 1.0, 1.0);   // el vidrio no absorbe color
	// Si venimos por fuera, eta/eta' = 1/n; si por dentro, n.
	double ri = rec.front_face ? (1.0 / mat->refraction_index) : mat->refraction_index;

	vec3 unit_direction = vec3_unit(r_in.dir);
	double cos_theta = fmin(vec3_dot(vec3_neg(unit_direction), rec.normal), 1.0);
	double sin_theta = sqrt(1.0 - cos_theta * cos_theta);

	int cannot_refract = (ri * sin_theta > 1.0);   // reflexion total interna
	vec3 direction;
	if (cannot_refract || reflectance(cos_theta, ri) > random_double())
	    direction = reflect(unit_direction, rec.normal);
	else
	    direction = refract(unit_direction, rec.normal, ri);

	scattered->orig = rec.p;
	scattered->dir  = direction;
	return 1;
    }
    }
    return 0;
}

#endif
