#include "wayland.h"

#include <assert.h>
#include <limits.h>
#include <stdint.h>
#include <string.h>
#include <sys/mman.h>
#include <time.h>
#include <unistd.h>
#include <wayland-client-protocol.h>
#include <wayland-util.h>
#include <xkbcommon/xkbcommon.h>

#include "helpers.h"
#include "keyboard.h"
#include "macros.h"
#include "pointer.h"
#include "xdg-shell-protocol.h"

static void
xdg_wm_base_handle_ping(void *data, struct xdg_wm_base *xdg_wm_base, u32 serial) {
    unused(xdg_wm_base);

    struct wayland *wayland = data;

    xdg_wm_base_pong(wayland->xdg_wm_base, serial);
}

static struct xdg_wm_base_listener xdg_wm_base_listener = {
        .ping = xdg_wm_base_handle_ping,
};

static void
registry_handle_global(void *data, struct wl_registry *registry, u32 name, const char *interface, u32 version) {
    struct wayland *wayland = data;

    if(strcmp(interface, wl_compositor_interface.name) == 0) {
        wayland->compositor = wl_registry_bind(registry, name, &wl_compositor_interface, version);
    } else if(strcmp(interface, wl_shm_interface.name) == 0) {
        wayland->memory_pool = memory_pool_create(wl_registry_bind(registry, name, &wl_shm_interface, version));
    } else if(strcmp(interface, wl_seat_interface.name) == 0) {
        wayland->seat = wl_registry_bind(registry, name, &wl_seat_interface, version);
    } else if(strcmp(interface, xdg_wm_base_interface.name) == 0) {
        wayland->xdg_wm_base = wl_registry_bind(registry, name, &xdg_wm_base_interface, version);
        xdg_wm_base_add_listener(wayland->xdg_wm_base, &xdg_wm_base_listener, NULL);
    }
}

static void
registry_handle_global_remove(void *data, struct wl_registry *registry, u32 name) {
    unused(data), unused(registry), unused(name);
}

static const struct wl_registry_listener registry_listener = {
        .global = registry_handle_global,
        .global_remove = registry_handle_global_remove,
};

static void
seat_handle_capabilities(void *data, struct wl_seat *seat, u32 capabilities) {
    struct wayland *wayland = data;

    if(capabilities & WL_SEAT_CAPABILITY_KEYBOARD) {
        keyboard_init(wayland, wl_seat_get_keyboard(seat));
    } else if(wayland->keyboard != NULL) {
        keyboard_deinit(wayland);
    }

    if(capabilities & WL_SEAT_CAPABILITY_POINTER) {
        pointer_init(wayland, wl_seat_get_pointer(seat));
    } else if(wayland->pointer != NULL) {
        pointer_deinit(wayland);
    }
}

static void
seat_handle_name(void *data, struct wl_seat *seat, const char *name) {
    unused(data), unused(seat), unused(name);
}

static const struct wl_seat_listener seat_listener = {
        .capabilities = seat_handle_capabilities,
        .name = seat_handle_name,
};

static u32
timespec_to_ms(struct timespec *ts) {
    return (u32)ts->tv_sec * 1000 + (u32)ts->tv_nsec / 1000000;
}

static void
draw_frame(struct wayland *wayland);

static void
handle_frame(void *data, struct wl_callback *callback, u32 callback_data) {
    unused(callback_data);

    wl_callback_destroy(callback);

    struct wayland *wayland = data;
    draw_frame(wayland);
}

static const struct wl_callback_listener frame_listener = {
        .done = handle_frame,
};

static void
draw_frame(struct wayland *wayland) {
    wayland->buffer =
            buffer_create(wayland->memory_pool, wayland->surface_current.width, wayland->surface_current.height);

    struct timespec now;
    clock_gettime(CLOCK_MONOTONIC, &now);

    wayland->impl->frame(wayland->data, wayland->buffer->mem->data, wayland->surface_current.width,
            wayland->surface_current.height, time_delta_ms(&now, &wayland->last_frame));

    wayland->last_frame = now;

    // request the new frame callback
    struct wl_callback *frame = wl_surface_frame(wayland->surface);
    wl_callback_add_listener(frame, &frame_listener, wayland);

    wl_surface_damage_buffer(wayland->surface, 0, 0, INT_MAX, INT_MAX);
    wl_surface_attach(wayland->surface, wayland->buffer->wl_buffer, 0, 0);
    wl_surface_commit(wayland->surface);
}

static void
xdg_surface_handle_configure(void *data, struct xdg_surface *xdg_surface, u32 serial) {
    struct wayland *wayland = data;

    xdg_surface_ack_configure(xdg_surface, serial);

    wayland->surface_current = wayland->surface_pending;
    if(wayland->buffer == NULL)
        // this is the initial configure, so we should commit the first buffer
        draw_frame(wayland);
}

static const struct xdg_surface_listener xdg_surface_listener = {
        .configure = xdg_surface_handle_configure,
};

static void
xdg_toplevel_handle_configure(void *data, struct xdg_toplevel *xdg_toplevel, i32 width, i32 height,
        struct wl_array *states) {
    unused(data), unused(xdg_toplevel), unused(states);

    struct wayland *wayland = data;

    wayland->surface_pending = (struct surface_state){
            .width = width == 0 ? DEFAULT_WIDTH : width,
            .height = height == 0 ? DEFAULT_HEIGHT : height,
    };
}

static void
xdg_toplevel_handle_configure_bounds(void *data, struct xdg_toplevel *xdg_toplevel, i32 width, i32 height) {
    unused(data), unused(xdg_toplevel), unused(width), unused(height);
}

static void
xdg_toplevel_handle_close(void *data, struct xdg_toplevel *toplevel) {
    unused(toplevel);

    struct wayland *wayland = data;
    wayland->impl->close(data);
}

static void
xdg_toplevel_handle_wm_capabilities(void *data, struct xdg_toplevel *xdg_toplevel, struct wl_array *capabilities) {
    unused(data), unused(xdg_toplevel), unused(capabilities);
}

static const struct xdg_toplevel_listener xdg_toplevel_listener = {
        .configure = xdg_toplevel_handle_configure,
        .configure_bounds = xdg_toplevel_handle_configure_bounds,
        .close = xdg_toplevel_handle_close,
        .wm_capabilities = xdg_toplevel_handle_wm_capabilities,
};

struct wayland *
wayland_create(struct wayland_impl *impl, void *data) {
    struct wayland *wayland = calloc(1, sizeof(*wayland));
    wayland->impl = impl;
    wayland->data = data;

    wayland->display = wl_display_connect(NULL);
    assert(wayland->display != NULL && "couldn't connect to wayland server!\n");

    wayland->registry = wl_display_get_registry(wayland->display);
    assert(wayland->display != NULL && "couldn't bind to registry!\n");

    wl_registry_add_listener(wayland->registry, &registry_listener, wayland);
    wl_display_roundtrip(wayland->display);

    assert(wayland->compositor != NULL && "couldn't bind to wl_compositor!\n");
    assert(wayland->xdg_wm_base != NULL && "couldn't bind to xdg_wm_base!\n");
    assert(wayland->memory_pool != NULL && "couldn't bind to wl_shm!\n");
    assert(wayland->seat != NULL && "couldn't bind to wl_seat");

    wl_seat_add_listener(wayland->seat, &seat_listener, wayland);

    wayland->surface = wl_compositor_create_surface(wayland->compositor);

    wayland->xdg_surface = xdg_wm_base_get_xdg_surface(wayland->xdg_wm_base, wayland->surface);
    xdg_surface_add_listener(wayland->xdg_surface, &xdg_surface_listener, wayland);

    wayland->toplevel = xdg_surface_get_toplevel(wayland->xdg_surface);
    xdg_toplevel_add_listener(wayland->toplevel, &xdg_toplevel_listener, wayland);

    xdg_toplevel_set_title(wayland->toplevel, "demo");
    xdg_toplevel_set_app_id(wayland->toplevel, "demo");

    // initial commit
    wl_surface_commit(wayland->surface);

    return wayland;
}

void
wayland_destroy(struct wayland *wayland) {
    xdg_toplevel_destroy(wayland->toplevel);
    xdg_surface_destroy(wayland->xdg_surface);
    wl_surface_destroy(wayland->surface);

    wl_registry_destroy(wayland->registry);
    wl_compositor_destroy(wayland->compositor);
    wl_seat_destroy(wayland->seat);

    wl_shm_destroy(wayland->memory_pool->shm);
    memory_pool_destroy(wayland->memory_pool);

    xdg_wm_base_destroy(wayland->xdg_wm_base);

    keyboard_deinit(wayland);
    pointer_deinit(wayland);

    wl_display_disconnect(wayland->display);
    free(wayland);
}
