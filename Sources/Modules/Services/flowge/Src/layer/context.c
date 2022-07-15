#include <layer/context.h>

layerContext_t* createContext(screen_t* screen) {
    layerContext_t* context = (layerContext_t *) malloc(sizeof(layerContext_t));
    context->screen = screen;
    context->layers = NULL;
    return context;
}

void addLayer(layerContext_t* context, layer_t* layer) {
    if (context->layers == NULL) {
        context->layers = (layer_t**) calloc(1, sizeof(layer_t*));
    } else {
        uint64_t len = sizeof(context->layers)/sizeof(layer_t*);
        layer_t** temp = (layer_t**) calloc(len+1, sizeof(layer_t*));
        temp[len] = layer;
        context->layers = temp;
    }
}

void renderContext(layerContext_t* context) {

}