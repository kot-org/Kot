#pragma once

#include <core/main.h>

typedef struct {
    uint8_t* buffer;
    uint32_t width;
    uint32_t height;
    uint32_t x;
    uint32_t y;
    bool show;
} window_t;

typedef struct {
    screen_t* screen;
    screen_t* backbuffer;
    window_t** windows;
} windowContext_t;

uint32_t autoX(screen_t* screen);
uint32_t autoY(screen_t* screen);
uint32_t autoWidth(screen_t* screen);
uint32_t autoHeight(screen_t* screen);

window_t* createWindow(uint32_t x, uint32_t y, uint32_t width, uint32_t height);

void showWindow(window_t* layer);
void hideWindow(window_t* layer);

void renderWindow(windowContext_t* context, window_t* layer);

bool isFullyOverriding(window_t* layer1, window_t* layer2);
bool isOverriding(window_t* layer1, window_t* layer2);