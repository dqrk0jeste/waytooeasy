#include "color.h"

#include <stdlib.h>

#include "ints.h"

u32
color_random(void) {
    return color_pack(255, rand() % 256, rand() % 256, rand() % 256);
}
