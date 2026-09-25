#ifndef COLOR_H
#define COLOR_H

#include <stdio.h>
#include "vec3.h"
#include "rtweekend.h"

// Escribe un pixel en formato PPM (tres enteros 0..255) en el flujo "out".
static void write_color(FILE *out, color pixel_color) {
    double r = pixel_color.x;
    double g = pixel_color.y;
    double b = pixel_color.z;

    // Pasamos de [0,1] a bytes [0,255]
    int ir = (int)(256 * clamp(r, 0.0, 0.999));
    int ig = (int)(256 * clamp(g, 0.0, 0.999));
    int ib = (int)(256 * clamp(b, 0.0, 0.999));

    fprintf(out, "%d %d %d\n", ir, ig, ib);
}

#endif
