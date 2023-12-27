#ifndef DEVCONSOLE_H
#define DEVCONSOLE_H

#include <stdint.h>

#include <global/console.h>

void devconsole_set_bg_color(uint32_t bg);
void devconsole_set_fg_color(uint32_t fg);

void cursor_update(void);

#endif