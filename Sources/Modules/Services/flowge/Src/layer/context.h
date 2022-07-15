#pragma once

#include <layer/layer.h>

layerContext_t* createContext(screen_t* screen);

void renderContext(layerContext_t* context);