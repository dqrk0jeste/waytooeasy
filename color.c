#include "color.h"

#include <stdlib.h>

uint32_t
color_random(void) {
    return color_pack(255, rand() % 256, rand() % 256, rand() % 256);
}
