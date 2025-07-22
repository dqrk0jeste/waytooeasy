#pragma once

#include <assert.h>
#include <math.h>

typedef struct vec2 {
    float x, y;
} vec2;

typedef struct mat2 {
    float m[2][2];
} mat2;

static inline vec2
vec2_add(vec2 a, vec2 b) {
    return (vec2){a.x + b.x, a.y + b.y};
}

static inline vec2
vec2_sub(vec2 a, vec2 b) {
    return (vec2){a.x - b.x, a.y - b.y};
}

static inline vec2
vec2_scale(vec2 v, float s) {
    return (vec2){v.x * s, v.y * s};
}

static inline float
vec2_dot(vec2 a, vec2 b) {
    return a.x * b.x + a.y * b.y;
}

static inline float
vec2_len(vec2 v) {
    return sqrtf(vec2_dot(v, v));
}

static inline vec2
vec2_normalize(vec2 v) {
    float len = vec2_len(v);
    if(len < 1e-6f) {
        return (vec2){0};
    }

    return vec2_scale(v, 1.0f / len);
}

static inline mat2
mat2_add(mat2 a, mat2 b) {
    return (mat2){{{a.m[0][0] + b.m[0][0], a.m[0][1] + b.m[0][1]}, {a.m[1][0] + b.m[1][0], a.m[1][1] + b.m[1][1]}}};
}

static inline mat2
mat2_sub(mat2 a, mat2 b) {
    return (mat2){{{a.m[0][0] - b.m[0][0], a.m[0][1] - b.m[0][1]}, {a.m[1][0] - b.m[1][0], a.m[1][1] - b.m[1][1]}}};
}

static inline mat2
mat2_mul(mat2 a, mat2 b) {
    return (mat2){{
            {a.m[0][0] * b.m[0][0] + a.m[0][1] * b.m[1][0], a.m[0][0] * b.m[0][1] + a.m[0][1] * b.m[1][1]},
            {a.m[1][0] * b.m[0][0] + a.m[1][1] * b.m[1][0], a.m[1][0] * b.m[0][1] + a.m[1][1] * b.m[1][1]},
    }};
}

static inline mat2
mat2_scale(float s, mat2 a) {
    return (mat2){{{s * a.m[0][0], s * a.m[0][1]}, {s * a.m[1][0], s * a.m[1][1]}}};
}

static inline mat2
mat2_transpose(mat2 a) {
    return (mat2){{{a.m[0][0], a.m[1][0]}, {a.m[0][1], a.m[1][1]}}};
}

static inline float
mat2_det(mat2 a) {
    return a.m[0][0] * a.m[1][1] - a.m[0][1] * a.m[1][0];
}

static inline mat2
mat2_identity(void) {
    return (mat2){{{1, 0}, {0, 1}}};
}

static inline mat2
mat2_inverse(mat2 a) {
    float det = mat2_det(a);
    if(fabsf(det) < 1e-6f) {
        return mat2_identity();
    }

    float inv_det = 1.0f / det;
    return (mat2){{{inv_det * a.m[1][1], -inv_det * a.m[0][1]}, {-inv_det * a.m[1][0], inv_det * a.m[0][0]}}};
}

static inline mat2
mat2_rotation(float angle) {
    float c = cosf(angle), s = sinf(angle);
    return (mat2){{{c, -s}, {s, c}}};
}

static inline vec2
mat2_mul_vec2(mat2 a, vec2 v) {
    return (vec2){a.m[0][0] * v.x + a.m[0][1] * v.y, a.m[1][0] * v.x + a.m[1][1] * v.y};
}
