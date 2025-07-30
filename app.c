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
handle_frame(struct buffer *buffer, u32 time) {
    unused(time);

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
handle_key(u32 raw, char *utf8, enum wl_keyboard_key_state state, u32 time) {
    unused(time);

    printf("key: raw = %u, utf8 = %s, state = %d\n", raw, utf8, state);
}

static void
handle_close(void) {
    event_loop_stop_and_destroy(app.event_loop);
}

static void
handle_motion(float x, float y, u32 time) {
    unused(time);

    printf("motion: x = %f, y = %f\n", x, y);
}

static void
handle_relative_motion(float dx, float dy, u32 time) {
    unused(time);

    printf("relative motion: dx = %f, dy = %f\n", dx, dy);
}

static void
handle_click(u32 button, enum wl_pointer_button_state state, u32 time) {
    unused(time);

    printf("click: button = %u, state = %d\n", button, state);
}

static void
handle_timer(void *data) {
    printf("timer: data = %s\n", (char *)data);
}

int
main(int argc, char **argv) {
    unused(argc), unused(argv);

    srand(getpid());
    app.event_loop = event_loop_create((struct wayland_impl){
            .close = handle_close,
            .frame = handle_frame,
            .key = handle_key,
            .motion = handle_motion,
            .relative_motion = handle_relative_motion,
            .click = handle_click,
    });

    event_loop_add_timer(app.event_loop, 5000, handle_timer, "5000");
    event_loop_add_timer(app.event_loop, 3000, handle_timer, "3000");
    event_loop_add_timer(app.event_loop, 4000, handle_timer, "4000");
    event_loop_add_timer(app.event_loop, 7000, handle_timer, "7000");

    event_loop_start(app.event_loop);
}
