#include <window/window.h>

uint32_t autoX(screen_t* screen) {
    return 10;
}

uint32_t autoY(screen_t* screen) {
    return 10;
}

uint32_t autoWidth(screen_t* screen) {
    return 100;
}

uint32_t autoHeight(screen_t* screen) {
    return 100;
}

bool isUpperField(uint32_t x1, uint32_t y1, uint32_t w, uint32_t h, uint32_t x2, uint32_t y2) {
    return (x1 < x2) && (y1 < y2) && (x2 > (x1+w)) && (y2 > (y1+h));
}


bool isLowerField(uint32_t x1, uint32_t y1, uint32_t w, uint32_t h, uint32_t x2, uint32_t y2) {
    return (x1 > x2) && (y1 > y2) && (x2 < (x1+w)) && (y2 < (y1+h));
}

bool isFullyOverriding(window_t* w1, window_t* w2){
    if ((w1->x==w2->x)&&(w1->y==w2->y)){
        if ((w1->x+w1->width==w1->y+w1->height)&&(w2->x+w2->width==w2->y+w2->height)){
            return true;
        }
    }
    if (isLowerField(w1->x, w1->y, w1->width, w1->height, w2->x, w2->y)) {
        if (isUpperField(w1->x+w1->width, w1->y+w1->height, w1->width, w1->height, w2->x+w2->width, w2->y+w2->height)) {
            return true;
        }
    }
    return false;
}

bool isOverriding(window_t* w1, window_t* w2){
    if ((w1->x==w2->x)&&(w1->y==w2->y)){
        return true;
    }
    if ((w1->x+w1->width==w1->y+w1->height)&&(w2->x+w2->width==w2->y+w2->height)){
        return true;
    }
    if (isLowerField(w1->x, w1->y, w1->width, w1->height, w2->x, w2->y)) {
        return true;
    }
    if (isUpperField(w1->x+w1->width, w1->y+w1->height, w1->width, w1->height, w2->x+w2->width, w2->y+w2->height)) {
        return true;
    }
    return false;
}

window_t* createWindow(uint32_t x, uint32_t y, uint32_t width, uint32_t height) {
    window_t* window = (window_t *) malloc(sizeof(window_t));
    window->x = x;
    window->y = y;
    window->width = width;
    window->height = height;
    window->show = false;
    return window;
}

void showWindow(window_t* window) {
    window->show = true;
}

void hideWindow(window_t* window) {
    window->show = false;
}

void renderWindow(windowContext_t* context, window_t* window) {
    if (window->show == true) {
        fillRect(context->backbuffer, window->x, window->y, window->width, window->height, 0xFFFFFF);
        drawRect(context->backbuffer, window->x, window->y, window->width, window->height, 0xFF0000);
    }
}