#include <layer/layer.h>

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

bool isFullyOverriding(layer_t* layer1, layer_t* layer2){
    if ((layer1->x==layer2->x)&&(layer1->y==layer2->y)){
        if ((layer1->x+layer1->width==layer1->y+layer1->height)&&(layer2->x+layer2->width==layer2->y+layer2->height)){
            return true;
        }
    }
    if (isLowerField(layer1->x, layer1->y, layer1->width, layer1->height, layer2->x, layer2->y)) {
        if (isUpperField(layer1->x+layer1->width, layer1->y+layer1->height, layer1->width, layer1->height, layer2->x+layer2->width, layer2->y+layer2->height)) {
            return true;
        }
    }
    return false;
}

bool isOverriding(layer_t* layer1, layer_t* layer2){
    if ((layer1->x==layer2->x)&&(layer1->y==layer2->y)){
        return true;
    }
    if ((layer1->x+layer1->width==layer1->y+layer1->height)&&(layer2->x+layer2->width==layer2->y+layer2->height)){
        return true;
    }
    if (isLowerField(layer1->x, layer1->y, layer1->width, layer1->height, layer2->x, layer2->y)) {
        return true;
    }
    if (isUpperField(layer1->x+layer1->width, layer1->y+layer1->height, layer1->width, layer1->height, layer2->x+layer2->width, layer2->y+layer2->height)) {
        return true;
    }
    return false;
}

layer_t* createLayer(uint32_t x, uint32_t y, uint32_t width, uint32_t height) {
    layer_t* layer = (layer_t *) malloc(sizeof(layer_t));
    layer->x = x;
    layer->y = y;
    layer->width = width;
    layer->height = height;
    layer->show = false;
    return layer;
}

void showLayer(layer_t* layer) {
    layer->show = true;
}

void hideLayer(layer_t* layer) {
    layer->show = false;
}

void renderLayer(layerContext_t* context, layer_t* layer) {
    if (layer->show == true) {
        fillRect(context->backbuffer, layer->x, layer->y, layer->width, layer->height, 0xFFFFFF);
        drawRect(context->backbuffer, layer->x, layer->y, layer->width, layer->height, 0xFF00FF);
    }
}