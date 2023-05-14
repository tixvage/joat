#include "lerp.h"

#include "hsv.h"

double lerpd(double start, double end, float amount) {
    double result = start + (end - start) * amount;
    return result;
}

Lerp new_lerp(Color start, Color end, float speed) {
    return (Lerp) { start, end, speed, 0.0f };
}

Color update_lerp(Lerp *lerp) {
    float rate = 1.0f/lerp->speed;
    Color color = lerp->start;
    if (lerp->i < 1.0) {
        lerp->i += GetFrameTime() * rate;
        HSV ca = rgb2hsv(lerp->start);
        HSV cb = rgb2hsv(lerp->end);
        HSV final = { 0 };
        final.h = lerpd(ca.h, cb.h, lerp->i);
        final.v = lerpd(ca.v, cb.v, lerp->i);
        final.s = lerpd(ca.s, cb.s, lerp->i);
        color = hsv2rgb(final);
    } else {
        lerp->i = 0;
        Color ba = lerp->end;
        lerp->end = lerp->start;
        lerp->start = ba;
        color = ba;
    }
    return color;
}

