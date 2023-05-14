#ifndef _LERP_H
#define _LERP_H

#include <raylib.h>

double lerpd(double start, double end, float amount);

typedef struct Lerp {
    Color start;
    Color end;
    float speed;
    float i;
} Lerp;

Lerp new_lerp(Color start, Color end, float speed);
Color update_lerp(Lerp *lerp);

#endif
