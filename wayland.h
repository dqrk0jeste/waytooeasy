#ifndef WAYLAND_H
#define WAYLAND_H

#include <stdbool.h>

#include "buffer.h"
#include "ints.h"
#include "keyboard.h"
#include "list.h"

#ifndef DEFAULT_WIDTH
#define DEFAULT_WIDTH 1024
#endif

#ifndef DEFAULT_HEIGHT
#define DEFAULT_HEIGHT 576
#endif

struct wayland_impl {
    void (*frame)(struct buffer *buffer, float dt);
    void (*key)(struct key key, uint time);
};

struct state {
    int width, height;
};

struct wayland {
    struct wayland_impl impl;
    struct event_loop *event_loop;

    struct wl_display *display;
    struct wl_registry *registry;

    struct wl_seat *seat;

    struct wl_pointer *pointer;
    float pointer_x, pointer_y;

    struct wl_keyboard *keyboard;
    struct xkb_context *xkb_context;
    struct xkb_keymap *xkb_keymap;
    struct xkb_state *xkb_state;

    struct wl_compositor *compositor;
    struct xdg_wm_base *xdg_wm_base;

    struct wl_surface *surface;
    struct xdg_surface *xdg_surface;
    struct xdg_toplevel *toplevel;

    struct memory_pool *memory_pool;

    struct buffer *buffer;

    struct state current, pending;
};

struct wayland *
wayland_create(struct event_loop *event_loop, struct wayland_impl impl);

void
wayland_destroy(struct wayland *wayland);

#endif
