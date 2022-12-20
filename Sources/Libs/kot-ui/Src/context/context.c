#include <kot-ui/context.h>

ContextUi_t* CreateUiContext(framebuffer_t* fb) {
    ContextUi_t* ctx = malloc(sizeof(ContextUi_t));

    ctx->fb = fb;

    return ctx; 
}