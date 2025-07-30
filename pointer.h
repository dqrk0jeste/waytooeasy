#ifndef POINTER_H
#define POINTER_H

#include <stdbool.h>
#include <wayland-client-protocol.h>

#include "ints.h"

struct wayland;

struct pointer_state {
    float x, y;

    float axis;
};

void
pointer_init(struct wayland *wayland, struct wl_pointer *pointer);

void
pointer_deinit(struct wayland *wayland);

#endif
