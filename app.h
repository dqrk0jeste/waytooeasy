#ifndef APP_H
#define APP_H

#include <stdbool.h>

struct app {
    struct event_loop *event_loop;
    float phase;
};

// piece of global state
extern struct app app;

#endif
