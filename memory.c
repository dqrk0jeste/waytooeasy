#include "memory.h"

#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <time.h>
#include <unistd.h>
#include <wayland-client-protocol.h>

#define LIST_IMPLEMENTATION
#include "list.h"

static void
get_random_name(char *dest) {
    struct timespec ts;
    clock_gettime(CLOCK_REALTIME, &ts);
    long r = ts.tv_nsec;
    for(int i = 0; i < 6; ++i) {
        dest[i] = 'A' + (r & 15) + (r & 16) * 2;
        r >>= 5;
    }
}

static int
create_shm_file(void) {
    int retries = 100;
    do {
        char name[] = "/wl_shm-XXXXXX";
        get_random_name(name + sizeof(name) - 7);
        retries--;

        int fd = shm_open(name, O_RDWR | O_CREAT | O_EXCL, 0600);
        if(fd >= 0) {
            shm_unlink(name);
            return fd;
        }
    } while(retries > 0 && errno == EEXIST);

    return -1;
}

static int
allocate_shm_file(int size) {
    int fd = create_shm_file();
    if(fd < 0) {
        return -1;
    }

    int ret;
    do {
        ret = ftruncate(fd, size);
    } while(ret < 0 && errno == EINTR);

    if(ret < 0) {
        close(fd);
        return -1;
    }

    return fd;
}

static struct memory_chunk *
memory_chunk_create(struct memory_pool *pool, int size) {
    int fd = allocate_shm_file(size);
    if(fd < 0) {
        close(fd);
        return NULL;
    }

    struct memory_chunk *chunk = calloc(1, sizeof(*chunk));
    chunk->size = size;
    chunk->pool = pool;
    chunk->wl_shm_pool = wl_shm_create_pool(pool->shm, fd, size);

    chunk->data = mmap(NULL, size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    if(chunk->data == MAP_FAILED) {
        close(fd);
        free(chunk);
        return NULL;
    }

    close(fd);

    return chunk;
}

struct memory_chunk *
memory_pool_get_chunk(struct memory_pool *pool, int size) {
    // lookup cached ones
    list_for_each_safe(iter, &pool->chunks) {
        struct memory_chunk *chunk = container_of(iter, chunk, link);
        if(size <= chunk->size) {
            // and return if there is a large enough chunk
            list_remove(&pool->chunks, &chunk->link);
            return chunk;
        }
    }

    // else create a new one
    return memory_chunk_create(pool, size);
}

static void
memory_chunk_destroy(struct memory_chunk *chunk) {
    wl_shm_pool_destroy(chunk->wl_shm_pool);
    munmap(chunk->data, chunk->size);

    free(chunk);
}

static void
insert_sorted(struct memory_pool *pool, struct memory_chunk *chunk) {
    // list is always sorted in the ascending order
    list_for_each_reverse(iter, &pool->chunks) {
        struct memory_chunk *iter_chunk = container_of(iter, iter_chunk, link);
        if(iter_chunk->size < chunk->size) {
            list_insert_after(&pool->chunks, &iter_chunk->link, &chunk->link);
            return;
        }
    }

    // if we went through the whole list, that means this is the smallest chunk, so we push it to front
    list_push_front(&pool->chunks, &chunk->link);
}

void
memory_chunk_put(struct memory_chunk *chunk) {
    struct memory_pool *pool = chunk->pool;

    // cache this chunk for later use
    if(pool->chunks.len < CACHED_COUNT) {
        insert_sorted(pool, chunk);
    } else {
        // cache this chunk if its larger than some other in cache
        struct memory_chunk *first = container_of(pool->chunks.first, first, link);
        if(first->size > chunk->size) {
            // this chunk is smaller than any
            memory_chunk_destroy(chunk);
        } else {
            list_remove(&pool->chunks, &first->link);
            memory_chunk_destroy(first);
            insert_sorted(pool, chunk);
        }
    }
}

struct memory_pool *
memory_pool_create(struct wl_shm *shm) {
    struct memory_pool *pool = calloc(1, sizeof(*pool));
    pool->shm = shm;

    return pool;
}

void
memory_pool_destroy(struct memory_pool *pool) {
    // destroy the cached memory chunks
    list_for_each_safe(iter, &pool->chunks) {
        struct memory_chunk *chunk = wl_container_of(iter, chunk, link);
        memory_chunk_destroy(chunk);
    }

    free(pool);
}
