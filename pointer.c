#include "pointer.h"

#include <stdio.h>

#include "macros.h"
#include "wayland.h"

static void
handle_enter(void *data, struct wl_pointer *pointer, uint serial, struct wl_surface *surface, wl_fixed_t x,
        wl_fixed_t y) {
    unused(pointer), unused(serial), unused(surface);

    struct wayland *wayland = data;

    wayland->pointer_x = wl_fixed_to_double(x);
    wayland->pointer_y = wl_fixed_to_double(y);
}

static void
handle_leave(void *data, struct wl_pointer *pointer, uint serial, struct wl_surface *surface) {
    unused(data), unused(pointer), unused(serial), unused(surface);
}

static void
handle_motion(void *data, struct wl_pointer *pointer, uint time, wl_fixed_t x, wl_fixed_t y) {
    unused(data), unused(pointer), unused(time);

    struct wayland *wayland = data;

    wayland->pointer_x = wl_fixed_to_double(x);
    wayland->pointer_y = wl_fixed_to_double(y);

    printf("%f, %f\n", wayland->pointer_x, wayland->pointer_y);
}

static void
handle_button(void *data, struct wl_pointer *pointer, uint serial, uint time, uint button, uint state) {
    // unused(data), unused(pointer), unused(time), unused(x), unused(y);
}

static void
handle_axis(void *data, struct wl_pointer *pointer, uint time, uint axis, wl_fixed_t value) {
    unused(data), unused(pointer), unused(time), unused(axis), unused(value);
}

static void
handle_frame(void *data, struct wl_pointer *pointer) {
    unused(data), unused(pointer);
}

static void
handle_axis_source(void *data, struct wl_pointer *pointer, uint axis_source) {
    unused(data), unused(pointer), unused(axis_source);
}

static void
handle_axis_stop(void *data, struct wl_pointer *pointer, uint time, uint axis) {
    unused(data), unused(pointer), unused(time), unused(axis);
}

static void
handle_axis_discrete(void *data, struct wl_pointer *pointer, uint axis, int discrete) {
    unused(data), unused(pointer), unused(axis), unused(discrete);
}

static void
handle_axis_value120(void *data, struct wl_pointer *pointer, uint axis, int value120) {
    unused(data), unused(pointer), unused(axis), unused(value120);
}

static void
handle_axis_relative_direction(void *data, struct wl_pointer *pointer, uint axis, uint direction) {
    unused(data), unused(pointer), unused(axis), unused(direction);
}

static const struct wl_pointer_listener pointer_listener = {
        .enter = handle_enter,
        .leave = handle_leave,
        .motion = handle_motion,
        .button = handle_button,
        .axis = handle_axis,
        .frame = handle_frame,
        .axis_source = handle_axis_source,
        .axis_stop = handle_axis_stop,
        .axis_discrete = handle_axis_discrete,
        .axis_value120 = handle_axis_value120,
        .axis_relative_direction = handle_axis_relative_direction,
};

void
pointer_init(struct wayland *wayland, struct wl_pointer *pointer) {
    wayland->pointer = pointer;
    wl_pointer_add_listener(pointer, &pointer_listener, wayland);
}

void
pointer_deinit(struct wayland *wayland) {
    if(wayland->pointer != NULL) {
        wl_pointer_destroy(wayland->pointer);
        wayland->pointer = NULL;
    }
}
