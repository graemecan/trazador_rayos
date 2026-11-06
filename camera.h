#ifndef CAMERA_H
#define CAMERA_H

#include <stdio.h>
#include "rtweekend.h"
#include "hittable.h"
#include "material.h"
#include "color.h"

typedef struct {
    // --- Parametros que fijas tu ---
    double aspect_ratio;        // ancho / alto
    int    image_width;         // pixeles de ancho
    int    samples_per_pixel;   // muestras por pixel (antialiasing)
    int    max_depth;           // rebotes maximos por rayo
    double vfov;                // campo de vision vertical, en grados
    point3 lookfrom;            // posicion de la camara
    point3 lookat;              // a donde mira
    vec3   vup;                 // "arriba" de la camara

    // --- Valores derivados (los calcula camera_initialize) ---
    int    image_height;
    double pixel_samples_scale;
    point3 center;
    point3 pixel00_loc;
    vec3   pixel_delta_u, pixel_delta_v;
    vec3   u, v, w;             // base ortonormal de la camara
} camera;

// Calcula todos los valores derivados a partir de los parametros.
static void camera_initialize(camera *cam) {
    cam->image_height = (int)(cam->image_width / cam->aspect_ratio);
    if (cam->image_height < 1) cam->image_height = 1;

    cam->pixel_samples_scale = 1.0 / cam->samples_per_pixel;
    cam->center = cam->lookfrom;

    // Dimensiones del viewport a partir del FOV y la distancia al objetivo
    double focal_length = vec3_length(vec3_sub(cam->lookfrom, cam->lookat));
    double theta = degrees_to_radians(cam->vfov);
    double h = tan(theta / 2);
    double viewport_height = 2 * h * focal_length;
    double viewport_width  = viewport_height *
                             ((double)cam->image_width / cam->image_height);

    // Base ortonormal (u derecha, v arriba, w hacia atras)
    cam->w = vec3_unit(vec3_sub(cam->lookfrom, cam->lookat));
    cam->u = vec3_unit(vec3_cross(cam->vup, cam->w));
    cam->v = vec3_cross(cam->w, cam->u);

    // Vectores del viewport
    vec3 viewport_u = vec3_scale(cam->u, viewport_width);
    vec3 viewport_v = vec3_scale(vec3_neg(cam->v), viewport_height);
    cam->pixel_delta_u = vec3_div(viewport_u, cam->image_width);
    cam->pixel_delta_v = vec3_div(viewport_v, cam->image_height);

    // Esquina superior izquierda y primer pixel
    point3 viewport_upper_left =
        vec3_sub(vec3_sub(vec3_sub(cam->center, vec3_scale(cam->w, focal_length)),
                          vec3_scale(viewport_u, 0.5)),
                 vec3_scale(viewport_v, 0.5));
    cam->pixel00_loc =
        vec3_add(viewport_upper_left,
                 vec3_scale(vec3_add(cam->pixel_delta_u, cam->pixel_delta_v), 0.5));
}

// Rayo hacia un punto aleatorio del pixel (i, j).
static ray get_ray(const camera *cam, int i, int j) {
    double px = random_double() - 0.5;
    double py = random_double() - 0.5;
    point3 pixel_sample =
        vec3_add(cam->pixel00_loc,
            vec3_add(vec3_scale(cam->pixel_delta_u, i + px),
                     vec3_scale(cam->pixel_delta_v, j + py)));
    ray r = { cam->center, vec3_sub(pixel_sample, cam->center) };
    return r;
}

// Color de un rayo, con rebotes (recursivo).
static color ray_color(ray r, int depth, hittable_list world) {
    if (depth <= 0) return v3(0, 0, 0);

    hit_record rec;
    if (world_hit(world, r, 0.001, INFINITY, &rec)) {
        ray scattered;
        color attenuation;
        if (scatter(rec.mat, r, rec, &attenuation, &scattered))
            return vec3_mul(attenuation, ray_color(scattered, depth - 1, world));
        return v3(0, 0, 0);
    }
    vec3 unit_direction = vec3_unit(r.dir);
    double a = 0.5 * (unit_direction.y + 1.0);
    return vec3_add(vec3_scale(v3(1,1,1), 1.0 - a),
                    vec3_scale(v3(0.5,0.7,1.0), a));
}

// Renderiza el mundo y escribe la imagen PPM por stdout.
static void camera_render(camera *cam, hittable_list world) {
    camera_initialize(cam);

    printf("P3\n%d %d\n255\n", cam->image_width, cam->image_height);
    for (int j = 0; j < cam->image_height; j++) {
        fprintf(stderr, "\rFilas restantes: %d ", cam->image_height - j);
        fflush(stderr);
        for (int i = 0; i < cam->image_width; i++) {
            color pixel = v3(0, 0, 0);
            for (int s = 0; s < cam->samples_per_pixel; s++) {
                ray r = get_ray(cam, i, j);
                pixel = vec3_add(pixel, ray_color(r, cam->max_depth, world));
            }
            write_color(stdout, vec3_scale(pixel, cam->pixel_samples_scale));
        }
    }
    fprintf(stderr, "\r¡Listo!                 \n");
}

#endif
