#pragma once

#include <core/main.h>

typedef struct {
    uint16_t screen; // not implemented yet (the screen id)
    uint32_t width;
    uint32_t height;
    uint32_t x;
    uint32_t y;
} window_t;