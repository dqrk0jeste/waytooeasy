#ifndef EVENT_LOOP_H
#define EVENT_LOOP_H

#include <poll.h>
#include <wayland-client-protocol.h>

#include "array.h"
#include "list.h"
#include "wayland.h"

DEFINE_ARRAY_WITH_PREFIX(struct fd, fd)

struct event_loop {
    struct wayland *wayland;

    fd_array fds;
    list timers;

    bool running;
};

struct timer;

typedef void (*fd_handler)(void *data);
typedef void (*timer_handler)(void *data);

struct fd {
    int fd;

    fd_handler handler;
    void *data;
};

struct timer {
    uint32_t ends_at;

    timer_handler handler;
    void *data;

    list_node link;
};

struct event_loop *
event_loop_create(struct wayland_impl impl);

void
event_loop_destroy(struct event_loop *event_loop);

void
event_loop_start(struct event_loop *event_loop);

struct fd *
event_loop_add_fd(struct event_loop *event_loop, int fd, fd_handler handler, void *data);

void
event_loop_remove_fd(struct event_loop *event_loop, struct fd *fd);

struct timer *
event_loop_add_timer(struct event_loop *event_loop, int duration, timer_handler handler, void *data);

void
event_loop_remove_timer(struct event_loop *event_loop, struct timer *timer);

#endif
