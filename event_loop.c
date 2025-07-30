#include "event_loop.h"

#include <assert.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "list.h"
#include "wayland.h"

struct fd *
event_loop_add_fd(struct event_loop *event_loop, i32 fd, fd_handler handler, void *data) {
    struct fd *this = calloc(1, sizeof(*this));
    this->fd = fd;
    this->handler = handler;
    this->data = data;

    list_push_back(&event_loop->fds, &this->link);

    return this;
}

void
event_loop_remove_fd(struct event_loop *event_loop, struct fd *fd) {
    list_remove(&event_loop->fds, &fd->link);
    // handle polling or something
}

static u32
now_ms(void) {
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);

    return ts.tv_sec * 1000 + ts.tv_nsec / 1000000;
}

static void
insert_sorted(list *timers, struct timer *this) {
    list_for_each(iter, timers) {
        struct timer *timer = container_of(iter, timer, link);
        if(timer->ends_at > this->ends_at) {
            list_insert_before(timers, &timer->link, &this->link);
            return;
        }
    }

    list_push_back(timers, &this->link);
}

struct timer *
event_loop_add_timer(struct event_loop *event_loop, u32 duration_ms, timer_handler handler, void *data) {
    struct timer *this = calloc(1, sizeof(*this));
    this->ends_at = now_ms() + duration_ms;
    this->handler = handler;
    this->data = data;

    insert_sorted(&event_loop->timers, this);

    return this;
}

void
event_loop_remove_timer(struct event_loop *event_loop, struct timer *timer) {
    list_remove(&event_loop->timers, &timer->link);
    free(timer);
}

struct event_loop *
event_loop_create(struct wayland_impl impl) {
    struct event_loop *loop = calloc(1, sizeof(*loop));
    loop->wayland = wayland_create(loop, impl);

    return loop;
}

void
event_loop_stop_and_destroy(struct event_loop *event_loop) {
    // we just flag it for destruction when the callback returns
    event_loop->running = false;
}

void
event_loop_destroy(struct event_loop *event_loop) {
    list_for_each_safe(iter, &event_loop->fds) {
        free(iter);
    }

    list_for_each_safe(iter, &event_loop->timers) {
        free(iter);
    }

    wayland_destroy(event_loop->wayland);
    free(event_loop);
}

void
event_loop_start(struct event_loop *event_loop) {
    event_loop->running = true;

    struct wl_display *display = event_loop->wayland->display;
    i32 fd = wl_display_get_fd(display);
    struct pollfd pfd = {fd, POLLIN, 0};

    while(true) {
        i32 timeout = -1;
        if(event_loop->timers.len > 0) {
            struct timer *first = container_of(event_loop->timers.first, first, link);
            if(first->ends_at <= now_ms()) {
                first->handler(first->data);

                event_loop_remove_timer(event_loop, first);
                // we continue here so other timers could be proccessed before other events are
                continue;
            }

            // this means there is no expired timer now, so we can calculate the timeout for the poll
            timeout = now_ms() - first->ends_at;
        }

        while(wl_display_prepare_read(display) != 0) {
            wl_display_dispatch_pending(display);
            if(!event_loop->running)
                break;
        }

        if(wl_display_flush(display) < 0 && errno != EAGAIN) {
            fprintf(stderr, "failed to flush display: %s\n", strerror(errno));
            wl_display_cancel_read(display);
            break;
        }

        i32 ret = poll(&pfd, 1, timeout);
        if(ret == 0) {
            // timer timed out, so there should be a timer ready to be proccessed
            continue;
        } else if(ret < 0) {
            fprintf(stderr, "poll failed: %s\n", strerror(errno));
            wl_display_cancel_read(display);
            break;
        }

        if(pfd.revents & POLLIN) {
            if(wl_display_read_events(display) < 0) {
                fprintf(stderr, "failed to read events: %s\n", strerror(errno));
                break;
            }

            wl_display_dispatch_pending(display);
            if(!event_loop->running)
                break;
        } else {
            // is this needed?
            wl_display_cancel_read(display);
        }

        if(wl_display_get_error(display) != 0) {
            fprintf(stderr, "protocol error: %s\n", strerror(errno));
            break;
        }
    }

    event_loop_destroy(event_loop);
}
