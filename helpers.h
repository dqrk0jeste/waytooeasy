#ifndef HELPERS_H
#define HELPERS_H

#include <time.h>

#include "ints.h"

static inline u32
time_from_timespec_ms(struct timespec *ts) {
    return ts->tv_sec * 1000 + ts->tv_nsec / 1000000;
}

static inline u64
time_from_timespec_ns(struct timespec *ts) {
    return ts->tv_sec * 1000000000 + ts->tv_nsec;
}

static inline u32
time_now_ms(void) {
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);

    return time_from_timespec_ms(&ts);
}

static inline float
time_delta_ms(struct timespec *a, struct timespec *b) {
    return ((double)time_from_timespec_ns(a) - (double)time_from_timespec_ns(b)) / 1000000.0F;
}

#endif
