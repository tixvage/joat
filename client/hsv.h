#ifndef _HSV_H
#define _HSV_H

#include <raylib.h>

typedef struct HSV {
    double h;
    double s;
    double v;
} HSV;

HSV rgb2hsv(Color in);
Color hsv2rgb(HSV in);

#endif
