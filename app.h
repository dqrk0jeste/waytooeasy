#ifndef APP_H
#define APP_H

#include <stdbool.h>

struct app {
    struct event_loop *event_loop;
    float phase;

    struct timer *timer;
};

// piece of global state
extern struct app app;

#endif
