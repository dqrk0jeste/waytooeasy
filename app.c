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
#include "color.h"
#include "event_loop.h"
#include "macros.h"
#include "vec2.h"

#define lerp(a, b, t) (a + (b - a) * t)

struct app app = {0};

uint
color_lerp(uint a, uint b, float t) {
    return color_pack(lerp(color_get_a(a), color_get_a(b), t), lerp(color_get_r(a), color_get_r(b), t),
            lerp(color_get_g(a), color_get_g(b), t), lerp(color_get_b(a), color_get_b(b), t));
}

uint
radial(vec2 pos, float phase) {
    float angle = atan2f(pos.y, pos.x);

    return color_lerp(0xff0000ff, 0xff00ff00, (angle - phase) / (2 * PI));
}

static void
handle_frame(struct buffer *buffer, float dt) {
    unused(dt);

    // memset(buffer->mem->data, color_random(), buffer->width * buffer->height * 4);

    for(int x = 0; x < buffer->width; x++) {
        for(int y = 0; y < buffer->height; y++) {
            vec2 r = {x - buffer->width / 2.0f, y - buffer->height / 2.0f};
            buffer->mem->data[y * buffer->width + x] = radial(r, app.phase);
        }
    }

    app.phase += 0.1f;
    if(app.phase > 2 * PI)
        app.phase -= 2 * PI;
}

static void
handle_key(struct key key, uint time) {
    unused(time);
    char *pressed = key.state == WL_KEYBOARD_KEY_STATE_PRESSED ? "pressed" : "released";
    printf("%s %s\n", key.utf8, pressed);
}

int
main(int argc, char **argv) {
    unused(argc), unused(argv);

    srand(getpid());
    struct event_loop *loop = event_loop_create((struct wayland_impl){
            .frame = handle_frame,
            .key = handle_key,
    });

    event_loop_start(loop);
}
