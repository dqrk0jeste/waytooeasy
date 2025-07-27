#ifndef APP_H
#define APP_H

#include <stdbool.h>

#include "ints.h"

struct app {
    float phase;
};

// piece of global state
extern struct app app;

#endif
