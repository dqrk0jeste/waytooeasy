#ifndef OPTIONAL_H
#define OPTIONAL_H

#define DEFINE_OPTIONAL(T) DEFINE_OPTIONAL_WITH_PREFIX(T, T)

#define DEFINE_OPTIONAL_WITH_PREFIX(T, PREFIX) \
    typedef struct PREFIX##_result {           \
        bool has_value;                        \
        T value;                               \
    } PREFIX##_optional;

#endif
