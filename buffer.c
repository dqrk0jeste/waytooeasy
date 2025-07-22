#include "buffer.h"

#include <fcntl.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <unistd.h>
#include <wayland-client-protocol.h>

static void
buffer_handle_release(void *data, struct wl_buffer *wl_buffer) {
    struct buffer *buffer = data;
    wl_buffer_destroy(wl_buffer);

    buffer_destroy(buffer);
}

static const struct wl_buffer_listener wl_buffer_listener = {
        .release = buffer_handle_release,
};

struct buffer *
buffer_create(struct memory_pool *pool, int width, int height) {
    struct buffer *buffer = calloc(1, sizeof(*buffer));
    buffer->width = width;
    buffer->height = height;

    int stride = width * 4;
    int size = stride * height;
    buffer->mem = memory_pool_get_chunk(pool, size);

    buffer->wl_buffer =
            wl_shm_pool_create_buffer(buffer->mem->wl_shm_pool, 0, width, height, stride, WL_SHM_FORMAT_ARGB8888);

    wl_buffer_add_listener(buffer->wl_buffer, &wl_buffer_listener, buffer);

    return buffer;
}

void
buffer_destroy(struct buffer *buffer) {
    memory_chunk_put(buffer->mem);
    free(buffer);
}
