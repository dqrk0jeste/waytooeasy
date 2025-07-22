#ifndef ARGB_H
#define ARGB_H

#include <stdint.h>

static inline uint32_t
color_pack(uint8_t a, uint8_t r, uint8_t g, uint8_t b) {
    return ((uint32_t)a << 24) | ((uint32_t)r << 16) | ((uint32_t)g << 8) | b;
}

static inline uint8_t
color_get_a(uint32_t color) {
    return (color >> 24) & 0xFF;
}

static inline uint8_t
color_get_r(uint32_t color) {
    return (color >> 16) & 0xFF;
}

static inline uint8_t
color_get_g(uint32_t color) {
    return (color >> 8) & 0xFF;
}

static inline uint8_t
color_get_b(uint32_t color) {
    return color & 0xFF;
}

uint32_t
color_random(void);

#endif
