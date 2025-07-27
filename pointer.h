#ifndef POINTER_H
#define POINTER_H

#include <wayland-client-protocol.h>

struct wayland;

void
pointer_init(struct wayland *wayland, struct wl_pointer *pointer);

void
pointer_deinit(struct wayland *wayland);

#endif
