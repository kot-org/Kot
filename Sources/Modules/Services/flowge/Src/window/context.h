#pragma once

#include <window/window.h>

windowContext_t* createContext(screen_t* screen);

void renderContext(windowContext_t* context);
void addWindow(windowContext_t* context, window_t* layer);