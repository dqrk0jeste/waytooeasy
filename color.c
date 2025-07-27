#include "color.h"

#include <stdlib.h>

#include "ints.h"

uint
color_random(void) {
    return color_pack(255, rand() % 256, rand() % 256, rand() % 256);
}
