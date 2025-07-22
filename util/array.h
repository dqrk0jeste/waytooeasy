#ifndef ARRAY_H
#define ARRAY_H

#include <stdlib.h>

#define DEFINE_ARRAY(T) DEFINE_ARRAY_WITH_PREFIX(T, T)

#define DEFINE_ARRAY_WITH_PREFIX(T, PREFIX)                               \
    typedef struct PREFIX##_array {                                       \
        int len, cap;                                                     \
        T *data;                                                          \
    } PREFIX##_array;                                                     \
                                                                          \
    void PREFIX##_array_push(PREFIX##_array *array, T elem);              \
    void PREFIX##_array_remove(PREFIX##_array *array, int index);         \
    void PREFIX##_array_remove_fast(PREFIX##_array *array, int index);    \
    void PREFIX##_array_insert(PREFIX##_array *array, int index, T elem); \
    void PREFIX##_array_destroy(PREFIX##_array *array);                   \
    T *PREFIX##_array_end(PREFIX##_array *array);                         \
    T *PREFIX##_array_last(PREFIX##_array *array);

#define IMPLEMENT_ARRAY(T) IMPLEMENT_ARRAY_WITH_PREFIX(T, T)

#define IMPLEMENT_ARRAY_WITH_PREFIX(T, PREFIX)                             \
    void PREFIX##_array_push(PREFIX##_array *array, T elem) {              \
        if(array->cap == 0) {                                              \
            array->cap = 16;                                               \
            array->len = 0;                                                \
            array->data = malloc(array->cap * sizeof(T));                  \
        } else if(array->cap == array->len) {                              \
            array->cap *= 2;                                               \
            array->data = realloc(array->data, array->cap * sizeof(T));    \
        }                                                                  \
                                                                           \
        array->data[array->len] = elem;                                    \
        array->len++;                                                      \
    }                                                                      \
                                                                           \
    void PREFIX##_array_remove(PREFIX##_array *array, int index) {         \
        for(int i = index; i < array->len - 1; i++) {                      \
            array->data[i] = array->data[i + 1];                           \
        }                                                                  \
                                                                           \
        array->len--;                                                      \
    }                                                                      \
                                                                           \
    void PREFIX##_array_remove_fast(PREFIX##_array *array, int index) {    \
        array[index] = array[array->len - 1];                              \
        array->len--;                                                      \
    }                                                                      \
                                                                           \
    void PREFIX##_array_insert(PREFIX##_array *array, int index, T elem) { \
        if(array->cap == 0) {                                              \
            array->cap = 16;                                               \
            array->len = 0;                                                \
            array->data = malloc(array->cap * sizeof(T));                  \
        } else if(array->cap == array->len) {                              \
            array->cap *= 2;                                               \
            array->data = realloc(array->data, array->cap * sizeof(T));    \
        }                                                                  \
                                                                           \
        for(int i = array->len - 1; i >= index; i--) {                     \
            array->data[i + 1] = array->data[i];                           \
        }                                                                  \
                                                                           \
        array->data[index] = elem;                                         \
        array->len++;                                                      \
    }                                                                      \
                                                                           \
    void PREFIX##_array_destroy(PREFIX##_array *array) {                   \
        if(array->cap > 0) {                                               \
            free(array->data);                                             \
        }                                                                  \
    }                                                                      \
                                                                           \
    T *PREFIX##_array_end(PREFIX##_array *array) {                         \
        return &array->data[array->len];                                   \
    }                                                                      \
    T *PREFIX##_array_last(PREFIX##_array *array) {                        \
        return &array->data[array->len - 1];                               \
    }

#endif
