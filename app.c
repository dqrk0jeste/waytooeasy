#include "app.h"

#include <assert.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <unistd.h>
#include <wayland-client.h>

#include "buffer.h"
#include "camera.h"
#include "color.h"
#include "event_loop.h"
#include "macros.h"
#include "mesh.h"

struct app app = {0};

#define lerp(a, b, t) (a + (b - a) * t)

uint32_t
color_lerp(uint32_t a, uint32_t b, float t) {
    return color_pack(lerp(color_get_a(a), color_get_a(b), t), lerp(color_get_r(a), color_get_r(b), t),
            lerp(color_get_g(a), color_get_g(b), t), lerp(color_get_b(a), color_get_b(b), t));
}

static void
draw_frame(struct buffer *buffer, float dt) {
    memset(buffer->mem->data, 0xff, buffer->width * buffer->height * 4);
}

int
main(int argc, char **argv) {
    srand(getpid());

    struct event_loop *loop = event_loop_create((struct wayland_impl){
            .frame = draw_frame,
    });

    event_loop_start(loop);
}
