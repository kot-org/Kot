#include <kot-ui/context.h>

ContextUi_t* CreateUiContext(kot_framebuffer_t* fb) {
    ContextUi_t* ctx = malloc(sizeof(ContextUi_t));

    ctx->fb = fb;

    return ctx; 
}