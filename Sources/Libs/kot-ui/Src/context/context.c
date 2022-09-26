#include <kot-ui/context.h>

ctxui_t* CreateContextUi(framebuffer_t* fb) {
    ctxui_t* ctx = malloc(sizeof(ctxui_t));

    ctx->fb = fb;

    return ctx; 
}