#pragma once

#include <core/main.h>

typedef struct {
    uint8_t* buffer;
    uint32_t width;
    uint32_t height;
    uint32_t x;
    uint32_t y;
    bool show;
} layer_t;

typedef struct {
    screen_t* screen;
    screen_t* backbuffer;
    layer_t** layers;
} layerContext_t;

uint32_t autoX(screen_t* screen);
uint32_t autoY(screen_t* screen);
uint32_t autoWidth(screen_t* screen);
uint32_t autoHeight(screen_t* screen);

layer_t* createLayer(uint32_t x, uint32_t y, uint32_t width, uint32_t height);

void showLayer(layer_t* layer);
void hideLayer(layer_t* layer);

void renderLayer(layerContext_t* context, layer_t* layer);

bool isFullyOverriding(layer_t* layer1, layer_t* layer2);
bool isOverriding(layer_t* layer1, layer_t* layer2);