#include "wayland.h"

#include <assert.h>
#include <stdint.h>
#include <string.h>
#include <wayland-client-protocol.h>
#include <wayland-util.h>

#include "event_loop.h"
#include "xdg-shell-protocol.h"

static void
xdg_wm_base_handle_ping(void *data, struct xdg_wm_base *xdg_wm_base, uint32_t serial) {
    struct wayland *wayland = data;

    xdg_wm_base_pong(wayland->xdg_wm_base, serial);
}

static struct xdg_wm_base_listener xdg_wm_base_listener = {
        .ping = xdg_wm_base_handle_ping,
};

static void
registry_handle_global(void *data, struct wl_registry *registry, uint32_t name, const char *interface,
        uint32_t version) {
    struct wayland *wayland = data;

    if(strcmp(interface, wl_compositor_interface.name) == 0) {
        wayland->compositor = wl_registry_bind(registry, name, &wl_compositor_interface, version);
    } else if(strcmp(interface, wl_shm_interface.name) == 0) {
        wayland->memory_pool = memory_pool_create(wl_registry_bind(registry, name, &wl_shm_interface, version));
    } else if(strcmp(interface, xdg_wm_base_interface.name) == 0) {
        wayland->xdg_wm_base = wl_registry_bind(registry, name, &xdg_wm_base_interface, version);
        xdg_wm_base_add_listener(wayland->xdg_wm_base, &xdg_wm_base_listener, NULL);
    }
}

static void
registry_handle_global_remove(void *data, struct wl_registry *registry, uint32_t name) {
    // do nothing
}

static const struct wl_registry_listener registry_listener = {
        .global = registry_handle_global,
        .global_remove = registry_handle_global_remove,
};

static void
xdg_surface_handle_configure(void *data, struct xdg_surface *xdg_surface, uint32_t serial) {
    struct wayland *wayland = data;

    xdg_surface_ack_configure(xdg_surface, serial);

    wayland->current = wayland->pending;

    wayland->buffer = buffer_create(wayland->memory_pool, wayland->current.width, wayland->current.height);
    wayland->impl.frame(wayland->buffer, 0);

    wl_surface_attach(wayland->surface, wayland->buffer->wl_buffer, 0, 0);
    wl_surface_commit(wayland->surface);
}

static const struct xdg_surface_listener xdg_surface_listener = {
        .configure = xdg_surface_handle_configure,
};

static void
xdg_toplevel_handle_configure(void *data, struct xdg_toplevel *xdg_toplevel, int32_t width, int32_t height,
        struct wl_array *states) {
    struct wayland *wayland = data;

    wayland->pending = (struct state){
            .width = width == 0 ? DEFAULT_WIDTH : width,
            .height = height == 0 ? DEFAULT_HEIGHT : height,
    };
}

static void
xdg_toplevel_handle_configure_bounds(void *data, struct xdg_toplevel *xdg_toplevel, int32_t width, int32_t height) {
    // noop
}

static void
xdg_toplevel_handle_close(void *data, struct xdg_toplevel *toplevel) {
    struct wayland *wayland = data;

    wayland->event_loop->running = false;
}

static void
xdg_toplevel_handle_wm_capabilities(void *data, struct xdg_toplevel *xdg_toplevel, struct wl_array *capabilities) {
    // noop
}

static const struct xdg_toplevel_listener xdg_toplevel_listener = {
        .configure = xdg_toplevel_handle_configure,
        .configure_bounds = xdg_toplevel_handle_configure_bounds,
        .close = xdg_toplevel_handle_close,
        .wm_capabilities = xdg_toplevel_handle_wm_capabilities,
};

struct wayland *
wayland_create(struct event_loop *event_loop, struct wayland_impl impl) {
    struct wayland *wayland = calloc(1, sizeof(*wayland));
    wayland->impl = impl;
    wayland->event_loop = event_loop;

    wayland->display = wl_display_connect(NULL);
    wayland->registry = wl_display_get_registry(wayland->display);
    wl_registry_add_listener(wayland->registry, &registry_listener, wayland);
    wl_display_roundtrip(wayland->display);

    assert(wayland->compositor != NULL);
    assert(wayland->xdg_wm_base != NULL);
    assert(wayland->memory_pool != NULL);

    wayland->surface = wl_compositor_create_surface(wayland->compositor);

    wayland->xdg_surface = xdg_wm_base_get_xdg_surface(wayland->xdg_wm_base, wayland->surface);
    xdg_surface_add_listener(wayland->xdg_surface, &xdg_surface_listener, wayland);

    wayland->toplevel = xdg_surface_get_toplevel(wayland->xdg_surface);
    xdg_toplevel_add_listener(wayland->toplevel, &xdg_toplevel_listener, wayland);

    xdg_toplevel_set_title(wayland->toplevel, "rasterizer");
    xdg_toplevel_set_app_id(wayland->toplevel, "rasterizer");

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
    wl_shm_destroy(wayland->memory_pool->shm);
    memory_pool_destroy(wayland->memory_pool);
    xdg_wm_base_destroy(wayland->xdg_wm_base);

    wl_display_disconnect(wayland->display);

    free(wayland);
}
