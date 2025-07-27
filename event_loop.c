#include "event_loop.h"

#include <assert.h>
#include <stdlib.h>
#include <time.h>

#include "list.h"
#include "wayland.h"

IMPLEMENT_ARRAY_WITH_PREFIX(struct fd, fd)

struct fd *
event_loop_add_fd(struct event_loop *event_loop, int fd, fd_handler handler, void *data) {
    // FIXME: this is wrong
    fd_array_push(&event_loop->fds,
            (struct fd){
                    .fd = fd,
                    .handler = handler,
                    .data = data,
            });

    // return the last one
    return &event_loop->fds.data[event_loop->fds.len - 1];
}

void
event_loop_remove_fd(struct event_loop *event_loop, struct fd *fd) {
    // FIXME: this is wrong
    int index = fd - &event_loop->fds.data[0];
    assert(index > 0 && index < event_loop->fds.len);

    // since we dont care about the order of these
    fd_array_remove_fast(&event_loop->fds, index);
}

static uint32_t
now_ms(void) {
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);

    return ts.tv_sec * 1000 + ts.tv_nsec / 1000000;
}

struct timer *
event_loop_add_timer(struct event_loop *event_loop, int duration_ms, timer_handler handler, void *data) {
    struct timer *this = calloc(1, sizeof(*this));
    this->ends_at = now_ms() + duration_ms;
    this->handler = handler;
    this->data = data;

    // insert this timer in the right place, so the list is sorted
    struct timer *iter = container_of(event_loop->timers.first, iter, link);
    while(&iter->link != event_loop->timers.last && iter->ends_at < this->ends_at) {
        iter = container_of(&iter->link.next, iter, link);
    }

    list_insert_after(&event_loop->timers, &iter->link, &this->link);

    return this;
}

void
event_loop_remove_timer(struct event_loop *event_loop, struct timer *timer) {
    if(&timer->link == event_loop->timers.first) {
        // todo: dirty?
    }

    list_remove(&event_loop->timers, &timer->link);
    // todo: if started?
}

struct event_loop *
event_loop_create(struct wayland_impl impl) {
    struct event_loop *loop = calloc(1, sizeof(*loop));
    loop->wayland = wayland_create(loop, impl);

    return loop;
}

void
event_loop_destroy(struct event_loop *event_loop) {
    // todo: destroy wayland stuff
    fd_array_destroy(&event_loop->fds);
    list_for_each_safe(iter, &event_loop->timers) {
        free(iter);
    }

    free(event_loop);
}

void
event_loop_start(struct event_loop *event_loop) {
    event_loop->running = true;

    while(event_loop->running && wl_display_dispatch(event_loop->wayland->display))
        ;

    wayland_destroy(event_loop->wayland);

    free(event_loop);
}
