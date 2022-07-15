#include <layer/layer.h>

uint32_t autoX(screen_t* screen) {
    return 10;
}

uint32_t autoY(screen_t* screen) {
    return 10;
}

uint32_t autoWidth(screen_t* screen) {

}

uint32_t autoHeight(screen_t* screen) {

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

    

}