#ifndef MEMORY_H
#define MEMORY_H

#include <stdint.h>

#include "list.h"

#ifndef CACHED_COUNT
#define CACHED_COUNT 16
#endif

struct memory_pool {
    struct wl_shm *shm;

    // cached memory chunks
    list chunks;
};

struct memory_chunk {
    struct memory_pool *pool;
    struct wl_shm_pool *wl_shm_pool;

    uint32_t *data;
    int size;

    list_node link;
};

struct memory_pool *
memory_pool_create(struct wl_shm *wl_shm);

void
memory_pool_destroy(struct memory_pool *pool);

struct memory_chunk *
memory_pool_get_chunk(struct memory_pool *pool, int size);

void
memory_chunk_put(struct memory_chunk *chunk);

#endif
