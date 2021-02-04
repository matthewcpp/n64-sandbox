#ifndef VEC2_H
#define VEC2_H

typedef struct {
    int x, y;
} IVec2;

typedef struct {
    float x, y;
} Vec2;

void ivec2_add(IVec2* out, IVec2* a, IVec2* b);

#endif