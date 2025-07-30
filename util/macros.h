#ifndef MACROS_H
#define MACROS_H

#include <math.h>
#include <stdlib.h>

#define PI 3.14159265358979f

#define max(a, b) ((a) > (b) ? (a) : (b))
#define min(a, b) ((a) < (b) ? (a) : (b))
#define clamp(v, a, b) (max(min((v), (b)), (a))

#define fequal(f, v) (fabsf((f) - (v)) < 1e-6)

#define unused(x) ((void)x)

#endif
