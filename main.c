#include <stdio.h>
#include "vec3.h"
#include "color.h"
#include "ray.h"
#include <math.h>
#include "hittable.h"
#include "rtweekend.h"

ray get_ray(int i, int j, point3 pixel00_loc,
            vec3 pixel_delta_u, vec3 pixel_delta_v, point3 camera_center) {
    // Desplazamiento aleatorio en [-0.5, 0.5] dentro del pixel
    double px = random_double() - 0.5;
    double py = random_double() - 0.5;

    point3 pixel_sample =
        vec3_add(pixel00_loc,
            vec3_add(vec3_scale(pixel_delta_u, i + px),
                     vec3_scale(pixel_delta_v, j + py)));

    vec3 ray_direction = vec3_sub(pixel_sample, camera_center);
    ray r = { camera_center, ray_direction };
    return r;
}

color ray_color(ray r, int depth, hittable_list world) {
    // Caso base: demasiados rebotes, ya no se recoge mas luz.
    if (depth <= 0)
        return v3(0, 0, 0);

    hit_record rec;
    // OJO: tmin = 0.001, no 0, para evitar el "acne de sombra" (ver pista).
    if (world_hit(world, r, 0.001, INFINITY, &rec)) {
        // Nueva direccion difusa: normal + vector unitario aleatorio
        vec3 direction = vec3_add(rec.normal, random_unit_vector());
        ray bounced = { rec.p, direction };
        // Cada rebote conserva el 50% de la luz (reflectividad de prueba)
        return vec3_scale(ray_color(bounced, depth - 1, world), 0.5);
    }

    // Fondo (cielo)
    vec3 unit_direction = vec3_unit(r.dir);
    double a = 0.5 * (unit_direction.y + 1.0);
    return vec3_add(vec3_scale(v3(1,1,1), 1.0 - a),
                    vec3_scale(v3(0.5,0.7,1.0), a));
}

int main(void) {
    // --- Imagen ---
    double aspect_ratio = 16.0 / 9.0;
    int image_width  = 400;
    
    int max_depth = 50;
    
    int image_height = (int)(image_width / aspect_ratio);
    if (image_height < 1) image_height = 1;

    // --- Camara ---
    double focal_length    = 1.0;
    double viewport_height = 2.0;
    double viewport_width  = viewport_height * ((double)image_width / image_height);
    point3 camera_center   = v3(0, 0, 0);

    // Vectores que recorren el viewport (u: horizontal, v: vertical hacia abajo)
    vec3 viewport_u = v3(viewport_width, 0, 0);
    vec3 viewport_v = v3(0, -viewport_height, 0);

    // Separacion entre pixeles
    vec3 pixel_delta_u = vec3_div(viewport_u, image_width);
    vec3 pixel_delta_v = vec3_div(viewport_v, image_height);

    // Esquina superior izquierda del viewport y centro del primer pixel
    point3 viewport_upper_left =
        vec3_sub(vec3_sub(vec3_sub(camera_center, v3(0,0,focal_length)),
                          vec3_scale(viewport_u, 0.5)),
                 vec3_scale(viewport_v, 0.5));
    point3 pixel00_loc =
        vec3_add(viewport_upper_left,
                 vec3_scale(vec3_add(pixel_delta_u, pixel_delta_v), 0.5));

    sphere spheres[] = {
        { v3(0,      0, -1),   0.5, NULL },
        { v3(0, -100.5, -1), 100.0, NULL },
    };
    hittable_list world = { spheres, 2 };

    // --- Render ---
    printf("P3\n%d %d\n255\n", image_width, image_height);
    for (int j = 0; j < image_height; j++) {
        fprintf(stderr, "\rFilas restantes: %d ", image_height - j);
        fflush(stderr);
        for (int i = 0; i < image_width; i++) {
            int samples_per_pixel = 100;
            double pixel_samples_scale = 1.0 / samples_per_pixel;

            color pixel = v3(0, 0, 0);
            for (int s = 0; s < samples_per_pixel; s++) {
                ray r = get_ray(i, j, pixel00_loc, pixel_delta_u, pixel_delta_v, camera_center);
                pixel = vec3_add(pixel, ray_color(r, max_depth, world));
            }
            write_color(stdout, vec3_scale(pixel, pixel_samples_scale));
        }
    }
    fprintf(stderr, "\r¡Listo!                  \n");
    return 0;
}
