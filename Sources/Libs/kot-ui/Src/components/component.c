#include <kot-ui/component.h>

#include <kot/sys.h>

void UpdateContext(ctxg_t* ctx) {
/*     for(int i = 0; i < componentsList->length; i++) {
        component_t* cpnt = vector_get(componentsList, i);

        UpdateComponent(cpnt);
    } */
}

void UpdateComponent(component_t* component) {
    Printlog("ok");
}

component_t* AddComponent(component_t* parent) {
    component_t* cpnt = malloc(sizeof(component_t));

    cpnt->context = parent->context;
    cpnt->parent = parent;

    parent->childs = vector_create();
    vector_push(parent->childs, cpnt);

    return cpnt;
}

void RemoveComponent(component_t* cpnt) {
    // vector_remove(cpnt->componentsList, cpnt->index);
    free(cpnt);
}

component_t* GetMainParent(ctxui_t* ctx) {
    component_t* parent = malloc(sizeof(component_t));

    parent->context = ctx;

    return parent;
}

void DrawTitleBar(ctxg_t* ctx, uint32_t width, uint32_t height, uint32_t colour) {
    fillRect(ctx, 0, 0, width, height, colour);

    uint8_t tmpWidthIcon = 10;

    // _ [] X buttons (test)
    drawLine(ctx, ctx->width-(height+tmpWidthIcon)/2, (height+tmpWidthIcon)/2, ctx->width-(height-tmpWidthIcon)/2, (height-tmpWidthIcon)/2, 0xFFFFFF);
    drawLine(ctx, ctx->width-(height-tmpWidthIcon)/2, (height+tmpWidthIcon)/2, ctx->width-(height+tmpWidthIcon)/2, (height-tmpWidthIcon)/2, 0xFFFFFF);
    drawRect(ctx, (ctx->width-(height+tmpWidthIcon)/2)-25, (height-tmpWidthIcon)/2, tmpWidthIcon, tmpWidthIcon, 0xFFFFFF);
    drawLine(ctx, (ctx->width-(height+tmpWidthIcon)/2)-50, height/2, (ctx->width-(height+tmpWidthIcon)/2)-40, height/2, 0xFFFFFF);
}

titlebar_t* CreateTitleBar(titlebarparam_t param) {
    titlebar_t* tb = malloc(sizeof(titlebar_t));

    tb->title = param.title;
    tb->color = param.color;
    tb->visible = param.visible;
    tb->cpnt = AddComponent(param.parent);
    
    DrawTitleBar(param.parent->context->ctxg, param.parent->context->ctxg->width, 40, tb->color);

    return tb;
}
 
void test(component_t* parent) {
    AddComponent(parent);

    fillRect(parent->context->ctxg, 0, 0, 50, 20, 0xFF000030);
}