#ifndef DEVCONSOLE_H
#define DEVCONSOLE_H

#include <stdint.h>

#include <global/console.h>

void devconsole_set_bg_color(uint32_t bg);
uint32_t devconsole_get_bg_color(void);
void devconsole_set_fg_color(uint32_t fg);
uint32_t devconsole_get_fg_color(void);

void cursor_draw(void);
void cursor_remove(void);
void cursor_remove_last(void);
void cursor_update(void);

#endif