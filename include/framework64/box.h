#ifndef BOX_H
#define BOX_H

#include "vec3.h"

typedef struct {
    Vec3 min;
    Vec3 max;
} Box;

void box_center(Box* box, Vec3* out);

#endif