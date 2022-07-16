#include <window/context.h>

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

windowContext_t* createContext(screen_t* screen) {
    windowContext_t* context = (windowContext_t *) malloc(sizeof(windowContext_t));
    context->backbuffer = createBackbuffer(screen);
    context->screen = screen;
    context->windows = NULL;
    return context;
}

void addWindow(windowContext_t* context, window_t* layer) {
    if (context->windows == NULL) {
        context->windows = (window_t**) calloc(1, sizeof(window_t*));
        context->windows[0] = layer;
    } else {
        size_t len = sizeof(context->windows)/sizeof(window_t*);
        window_t** temp = (window_t**) calloc(len+1, sizeof(window_t*));
        for (size_t i = 0; i < len; i++) {
            temp[i] = context->windows[i];
        }
        free(context->windows);
        temp[len+1] = layer;
        context->windows = temp;
        free(temp);
    }
}

void renderContext(windowContext_t* context) {
    clear(context->backbuffer);
    uint64_t len = sizeof(context->windows)/sizeof(window_t*);
    for (size_t i = 0; i < len; i++) {
        renderWindow(context, context->windows[i]);
    }
    swapBuffers(context->backbuffer, context->screen);
}