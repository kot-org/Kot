#include <kot-ui/context.h>

ctxui_t* CreateContextUi(ctxg_t* ctxGraphic) {
    ctxui_t* ctx = malloc(sizeof(ctxui_t));

    ctx->ctxg = ctxGraphic;

    return ctx; 
}