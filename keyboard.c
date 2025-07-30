#include "keyboard.h"

#include <linux/input-event-codes.h>
#include <sys/mman.h>
#include <unistd.h>
#include <wayland-client-protocol.h>
#include <xkbcommon/xkbcommon.h>

#include "ints.h"
#include "macros.h"
#include "wayland.h"

static void
keyboard_handle_keymap(void *data, struct wl_keyboard *keyboard, u32 format, int fd, u32 size) {
    unused(keyboard);

    struct wayland *wayland = data;

    if(format != WL_KEYBOARD_KEYMAP_FORMAT_XKB_V1)
        goto error_fd;

    char *map = mmap(NULL, size, PROT_READ, MAP_PRIVATE, fd, 0);
    if(map == MAP_FAILED)
        goto error_fd;

    struct xkb_context *context = xkb_context_new(XKB_CONTEXT_NO_FLAGS);
    if(context == NULL)
        goto error_map;

    struct xkb_keymap *keymap =
            xkb_keymap_new_from_string(context, map, XKB_KEYMAP_FORMAT_TEXT_V1, XKB_KEYMAP_COMPILE_NO_FLAGS);
    if(keymap == NULL)
        goto error_context;

    struct xkb_state *state = xkb_state_new(keymap);
    if(state == NULL)
        goto error_keymap;

    if(wayland->xkb_state != NULL)
        xkb_state_unref(wayland->xkb_state);
    if(wayland->xkb_keymap != NULL)
        xkb_keymap_unref(wayland->xkb_keymap);
    if(wayland->xkb_context != NULL)
        xkb_context_unref(wayland->xkb_context);

    wayland->xkb_context = context;
    wayland->xkb_keymap = keymap;
    wayland->xkb_state = state;

    munmap(map, size);
    close(fd);
    return;

error_keymap:
    xkb_keymap_unref(keymap);
error_context:
    xkb_context_unref(context);
error_map:
    munmap(map, size);
error_fd:
    printf("keyboard keymap couldn't be loaded\n");
    close(fd);
}

static void
keyboard_handle_enter(void *data, struct wl_keyboard *wl_keyboard, u32 serial, struct wl_surface *surface,
        struct wl_array *keys) {
    unused(data), unused(wl_keyboard), unused(serial), unused(surface), unused(keys);
}

static void
keyboard_handle_leave(void *data, struct wl_keyboard *wl_keyboard, u32 serial, struct wl_surface *surface) {
    unused(data), unused(wl_keyboard), unused(serial), unused(surface);
}

static void
keyboard_handle_key(void *data, struct wl_keyboard *keyboard, u32 serial, u32 time, u32 key, u32 state) {
    unused(keyboard), unused(serial), unused(time);

    struct wayland *wayland = data;

    char buffer[8] = {0};
    if(wayland->xkb_state != NULL) {
        u32 keycode = key + 8;
        xkb_state_key_get_utf8(wayland->xkb_state, keycode, buffer, sizeof(buffer));
    }

    if(wayland->impl.key != NULL)
        wayland->impl.key(key, buffer, state, time);
}

static void
keyboard_handle_modifiers(void *data, struct wl_keyboard *keyboard, u32 serial, u32 mods_depressed, u32 mods_latched,
        u32 mods_locked, u32 group) {
    unused(keyboard), unused(serial);

    struct wayland *wayland = data;

    xkb_state_update_mask(wayland->xkb_state, mods_depressed, mods_latched, mods_locked, 0, 0, group);
}

static void
keyboard_handle_repeat_info(void *data, struct wl_keyboard *keyboard, int rate, int delay) {
    unused(data), unused(keyboard), unused(rate), unused(delay);
}

static const struct wl_keyboard_listener keyboard_listener = {
        .keymap = keyboard_handle_keymap,
        .enter = keyboard_handle_enter,
        .leave = keyboard_handle_leave,
        .key = keyboard_handle_key,
        .modifiers = keyboard_handle_modifiers,
        .repeat_info = keyboard_handle_repeat_info,
};

void
keyboard_init(struct wayland *wayland, struct wl_keyboard *wl_keyboard) {
    wayland->keyboard = wl_keyboard;
    wl_keyboard_add_listener(wl_keyboard, &keyboard_listener, wayland);
}

void
keyboard_deinit(struct wayland *wayland) {
    if(wayland->xkb_state != NULL) {
        xkb_state_unref(wayland->xkb_state);
        wayland->xkb_state = NULL;
    }
    if(wayland->xkb_keymap != NULL) {
        xkb_keymap_unref(wayland->xkb_keymap);
        wayland->xkb_keymap = NULL;
    }
    if(wayland->xkb_context != NULL) {
        xkb_context_unref(wayland->xkb_context);
        wayland->xkb_context = NULL;
    }

    if(wayland->keyboard != NULL) {
        wl_keyboard_destroy(wayland->keyboard);
        wayland->keyboard = NULL;
    }
}
