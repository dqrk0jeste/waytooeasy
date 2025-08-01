#ifndef EVENT_LOOP_H
#define EVENT_LOOP_H

#include <poll.h>
#include <wayland-client-protocol.h>

#include "list.h"
#include "wayland.h"

struct event_loop {
    struct wayland *wayland;

    list fds;
    list timers;

    bool running;
};

struct timer;

typedef void (*fd_handler)(void *data);
typedef void (*timer_handler)(void *data);

struct fd {
    i32 fd;

    fd_handler handler;
    void *data;

    list_node link;
};

struct timer {
    u32 ends_at;

    timer_handler handler;
    void *data;

    list_node link;
};

struct event_loop *
event_loop_create(void);

void
event_loop_start(struct event_loop *event_loop);

void
event_loop_stop_and_destroy(struct event_loop *event_loop);

void
event_loop_add_wayland(struct event_loop *event_loop, struct wayland_impl *impl, void *data);

struct fd *
event_loop_add_fd(struct event_loop *event_loop, i32 fd, fd_handler handler, void *data);

void
event_loop_remove_fd(struct event_loop *event_loop, struct fd *fd);

struct timer *
event_loop_add_timer(struct event_loop *event_loop, u32 duration, timer_handler handler, void *data);

void
event_loop_remove_timer(struct event_loop *event_loop, struct timer *timer);

#endif
