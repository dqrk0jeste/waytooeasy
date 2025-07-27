#pragma once

#include <assert.h>
#include <math.h>

typedef struct vec3 {
    float x, y, z;
} vec3;

typedef struct mat3 {
    float m[3][3];
} mat3;

static inline vec3
vec3_add(vec3 a, vec3 b) {
    return (vec3){a.x + b.x, a.y + b.y, a.z + b.z};
}

static inline vec3
vec3_sub(vec3 a, vec3 b) {
    return (vec3){a.x - b.x, a.y - b.y, a.z - b.z};
}

static inline vec3
vec3_scale(float s, vec3 a) {
    return (vec3){s * a.x, s * a.y, s * a.z};
}

static inline float
vec3_dot(vec3 a, vec3 b) {
    return a.x * b.x + a.y * b.y + a.z * b.z;
}

static inline float
vec3_len(vec3 a) {
    return sqrtf(vec3_dot(a, a));
}

static inline vec3
vec3_normalize(vec3 a) {
    float len = vec3_len(a);
    assert(len > 0);

    float inv_len = 1.0f / len;
    return (vec3){a.x * inv_len, a.y * inv_len, a.z * inv_len};
}

static inline vec3
vec3_cross(vec3 a, vec3 b) {
    return (vec3){a.y * b.z - a.z * b.y, a.z * b.x - a.x * b.z, a.x * b.y - a.y * b.x};
}

static inline mat3
mat3_add(mat3 a, mat3 b) {
    return (mat3){{
            {a.m[0][0] + b.m[0][0], a.m[0][1] + b.m[0][1], a.m[0][2] + b.m[0][2]},
            {a.m[1][0] + b.m[1][0], a.m[1][1] + b.m[1][1], a.m[1][2] + b.m[1][2]},
            {a.m[2][0] + b.m[2][0], a.m[2][1] + b.m[2][1], a.m[2][2] + b.m[2][2]},
    }};
}

static inline mat3
mat3_sub(mat3 a, mat3 b) {
    return (mat3){{
            {a.m[0][0] - b.m[0][0], a.m[0][1] - b.m[0][1], a.m[0][2] - b.m[0][2]},
            {a.m[1][0] - b.m[1][0], a.m[1][1] - b.m[1][1], a.m[1][2] - b.m[1][2]},
            {a.m[2][0] - b.m[2][0], a.m[2][1] - b.m[2][1], a.m[2][2] - b.m[2][2]},
    }};
}

static inline mat3
mat3_mul(mat3 a, mat3 b) {
    return (mat3){{
            {a.m[0][0] * b.m[0][0] + a.m[0][1] * b.m[1][0] + a.m[0][2] * b.m[2][0],
                    a.m[0][0] * b.m[0][1] + a.m[0][1] * b.m[1][1] + a.m[0][2] * b.m[2][1],
                    a.m[0][0] * b.m[0][2] + a.m[0][1] * b.m[1][2] + a.m[0][2] * b.m[2][2]},
            {a.m[1][0] * b.m[0][0] + a.m[1][1] * b.m[1][0] + a.m[1][2] * b.m[2][0],
                    a.m[1][0] * b.m[0][1] + a.m[1][1] * b.m[1][1] + a.m[1][2] * b.m[2][1],
                    a.m[1][0] * b.m[0][2] + a.m[1][1] * b.m[1][2] + a.m[1][2] * b.m[2][2]},
            {a.m[2][0] * b.m[0][0] + a.m[2][1] * b.m[1][0] + a.m[2][2] * b.m[2][0],
                    a.m[2][0] * b.m[0][1] + a.m[2][1] * b.m[1][1] + a.m[2][2] * b.m[2][1],
                    a.m[2][0] * b.m[0][2] + a.m[2][1] * b.m[1][2] + a.m[2][2] * b.m[2][2]},
    }};
}

static inline mat3
mat3_scale(float s, mat3 a) {
    return (mat3){{
            {s * a.m[0][0], s * a.m[0][1], s * a.m[0][2]},
            {s * a.m[1][0], s * a.m[1][1], s * a.m[1][2]},
            {s * a.m[2][0], s * a.m[2][1], s * a.m[2][2]},
    }};
}

static inline mat3
mat3_transpose(mat3 a) {
    return (mat3){{
            {a.m[0][0], a.m[1][0], a.m[2][0]},
            {a.m[0][1], a.m[1][1], a.m[2][1]},
            {a.m[0][2], a.m[1][2], a.m[2][2]},
    }};
}

static inline float
mat3_det(mat3 a) {
    return a.m[0][0] * (a.m[1][1] * a.m[2][2] - a.m[1][2] * a.m[2][1]) -
            a.m[0][1] * (a.m[1][0] * a.m[2][2] - a.m[1][2] * a.m[2][0]) +
            a.m[0][2] * (a.m[1][0] * a.m[2][1] - a.m[1][1] * a.m[2][0]);
}

static inline mat3
mat3_identity(void) {
    return (mat3){{{1, 0, 0}, {0, 1, 0}, {0, 0, 1}}};
}

static inline mat3
mat3_inverse(mat3 a) {
    float det = mat3_det(a);
    if(fabsf(det) < 1e-6f) {
        return mat3_identity();
    }

    float inv_det = 1.0f / det;
    return (mat3){{
            {inv_det * (a.m[1][1] * a.m[2][2] - a.m[1][2] * a.m[2][1]),
                    inv_det * (a.m[0][2] * a.m[2][1] - a.m[0][1] * a.m[2][2]),
                    inv_det * (a.m[0][1] * a.m[1][2] - a.m[0][2] * a.m[1][1])},
            {inv_det * (a.m[1][2] * a.m[2][0] - a.m[1][0] * a.m[2][2]),
                    inv_det * (a.m[0][0] * a.m[2][2] - a.m[0][2] * a.m[2][0]),
                    inv_det * (a.m[0][2] * a.m[1][0] - a.m[0][0] * a.m[1][2])},
            {inv_det * (a.m[1][0] * a.m[2][1] - a.m[1][1] * a.m[2][0]),
                    inv_det * (a.m[0][1] * a.m[2][0] - a.m[0][0] * a.m[2][1]),
                    inv_det * (a.m[0][0] * a.m[1][1] - a.m[0][1] * a.m[1][0])},
    }};
}

static inline mat3
mat3_rotation_x(float angle) {
    float c = cosf(angle), s = sinf(angle);
    return (mat3){{{1, 0, 0}, {0, c, -s}, {0, s, c}}};
}

static inline mat3
mat3_rotation_y(float angle) {
    float c = cosf(angle), s = sinf(angle);
    return (mat3){{{c, 0, s}, {0, 1, 0}, {-s, 0, c}}};
}

static inline mat3
mat3_rotation_z(float angle) {
    float c = cosf(angle), s = sinf(angle);
    return (mat3){{{c, -s, 0}, {s, c, 0}, {0, 0, 1}}};
}

static inline vec3
mat3_mul_vec3(mat3 a, vec3 v) {
    return (vec3){
            a.m[0][0] * v.x + a.m[0][1] * v.y + a.m[0][2] * v.z,
            a.m[1][0] * v.x + a.m[1][1] * v.y + a.m[1][2] * v.z,
            a.m[2][0] * v.x + a.m[2][1] * v.y + a.m[2][2] * v.z,
    };
}
