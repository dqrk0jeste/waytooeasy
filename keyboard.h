#ifndef KEYBOARD_H
#define KEYBOARD_H

#include <wayland-client-protocol.h>

struct wayland;

void
keyboard_init(struct wayland *wayland, struct wl_keyboard *wl_keyboard);

void
keyboard_deinit(struct wayland *wayland);

#endif
