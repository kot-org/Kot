#pragma once

#include <kot/sys.h>

void put_pixel(uint32_t x, uint32_t y, int r, int g, int b);

void create_rect(uint32_t x, uint32_t y, uint32_t w, uint32_t h, uint32_t color);

void create_Gradientrect(uint32_t x, uint32_t y, uint32_t w, uint32_t h, uint32_t fromColor, uint32_t toColor);