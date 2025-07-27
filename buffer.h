#ifndef BUFFER_H
#define BUFFER_H

#include <stddef.h>
#include <wayland-client-protocol.h>

#include "memory.h"

#ifndef CACHED_BUFFERS_COUNT
#define CACHED_BUFFERS_COUNT 4
#endif

struct buffer {
    struct wl_buffer *wl_buffer;
    struct memory_chunk *mem;

    int width, height;
};

struct buffer *
buffer_create(struct memory_pool *pool, int width, int height);

// destroys client-side buffer that cannot be in use (has to be released by the server)
void
buffer_destroy(struct buffer *buffer);

#endif
