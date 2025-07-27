#ifndef KEYBOARD_H
#define KEYBOARD_H

#include <wayland-client-protocol.h>

#include "ints.h"

struct wayland;

void
keyboard_init(struct wayland *wayland, struct wl_keyboard *wl_keyboard);

void
keyboard_deinit(struct wayland *wayland);

struct key {
    uint raw;
    char *utf8;

    enum wl_keyboard_key_state state;
};

#endif
