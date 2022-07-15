#include <layer/context.h>

void swapBuffers(screen_t* from, screen_t* to) {
    memcpy((uint8_t*) to->fb_addr, (uint8_t*) from->fb_addr, to->height * to->pitch);
}

screen_t* createBackbuffer(screen_t* screen) {
    screen_t* backbuffer = (screen_t *) malloc(sizeof(screen_t));
    backbuffer->fb_addr = (uint8_t *) malloc(screen->fb_size);
    backbuffer->fb_size = screen->fb_size;
    backbuffer->width = screen->width;
    backbuffer->height = screen->height;
    backbuffer->pitch = screen->pitch;
    backbuffer->bpp = screen->bpp;
    backbuffer->btpp = screen->btpp;
    backbuffer->bps = screen->bps;
    return backbuffer;
}

layerContext_t* createContext(screen_t* screen) {
    layerContext_t* context = (layerContext_t *) malloc(sizeof(layerContext_t));
    context->backbuffer = createBackbuffer(screen);
    context->screen = screen;
    context->layers = NULL;
    return context;
}

void addLayer(layerContext_t* context, layer_t* layer) {
    if (context->layers == NULL) {
        context->layers = (layer_t**) calloc(1, sizeof(layer_t*));
        context->layers[0] = layer;
    } else {
        uint64_t len = sizeof(context->layers)/sizeof(layer_t*);
        layer_t** temp = (layer_t**) calloc(len+1, sizeof(layer_t*));
        for (size_t i = 0; i < len-1; i++) {
            temp[i] = context->layers[i];
        }
        temp[len] = layer;
        context->layers = temp;
    }
}

void renderContext(layerContext_t* context) {
    clear(context->backbuffer);
    uint64_t len = sizeof(context->layers)/sizeof(layer_t*);
    for (size_t i = 0; i < len; i++) {
        renderLayer(context, context->layers[i]);
    }
    swapBuffers(context->backbuffer, context->screen);
}