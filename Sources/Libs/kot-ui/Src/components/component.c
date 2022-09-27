#include <kot-ui/component.h>

#include <kot/sys.h>

void UpdateContext(ctxui_t* ctx) {
/*     for(int i = 0; i < componentsList->length; i++) {
        component_t* cpnt = vector_get(componentsList, i);

        UpdateComponent(cpnt);
    } */
}

void UpdateComponent(component_t* component) {
    
}

void blitComponentFramebuffer(component_t* component) {
    while(component->parent != NULL) {
        blitFramebuffer(component->parent->fb, component->fb, component->x, component->y);
        component = component->parent;
    }
}

component_t* AddComponent(uint32_t width, uint32_t height, uint32_t xPos, uint32_t yPos, component_t* parent) {
    /* framebuffer */
    framebuffer_t* fb = malloc(sizeof(framebuffer_t));

    uint32_t pitch = width * parent->fb->btpp;

    fb->size = height * pitch;
    fb->addr = calloc(fb->size);
    fb->pitch = pitch;
    fb->width = width;
    fb->height = height;
    fb->bpp = parent->fb->bpp;
    fb->btpp = parent->fb->btpp;

    /* component */
    component_t* cpnt = malloc(sizeof(component_t));

    cpnt->fb = fb;
    cpnt->parent = parent;
    cpnt->x = xPos;
    cpnt->y = yPos;
    
    parent->childs = vector_create();
    vector_push(parent->childs, cpnt);

    /* char* buff[50];
    itoa(cpnt->fb->addr, buff, 16);
    Printlog(buff); */

    return cpnt;
}

void RemoveComponent(component_t* cpnt) {
    // vector_remove(cpnt->componentsList, cpnt->index);
    free(cpnt);
}

component_t* GetMainParent(framebuffer_t* fb) {
    component_t* parent = malloc(sizeof(component_t));

    parent->fb = fb;

    return parent;
}

/* Components */

canva_t* CreateCanva(uint32_t width, uint32_t height, uint32_t xPos, uint32_t yPos, component_t* parent) {
    canva_t* canva = malloc(sizeof(canva_t));

    canva->cpnt = AddComponent(width, height, xPos, yPos, parent); 

    return canva;
}

void DrawTitleBar(component_t* cpnt, uint32_t color) {
    fillRect(cpnt->fb, cpnt->x, cpnt->y, cpnt->fb->width, cpnt->fb->height, color);

/*     uint8_t tmpWidthIcon = 10;

    // _ [] X buttons (test)
    drawLine(ctx, ctx->width-(height+tmpWidthIcon)/2, (height+tmpWidthIcon)/2, ctx->width-(height-tmpWidthIcon)/2, (height-tmpWidthIcon)/2, 0xFFFFFF);
    drawLine(ctx, ctx->width-(height-tmpWidthIcon)/2, (height+tmpWidthIcon)/2, ctx->width-(height+tmpWidthIcon)/2, (height-tmpWidthIcon)/2, 0xFFFFFF);
    drawRect(ctx, (ctx->width-(height+tmpWidthIcon)/2)-25, (height-tmpWidthIcon)/2, tmpWidthIcon, tmpWidthIcon, 0xFFFFFF);
    drawLine(ctx, (ctx->width-(height+tmpWidthIcon)/2)-50, height/2, (ctx->width-(height+tmpWidthIcon)/2)-40, height/2, 0xFFFFFF);  */
    blitComponentFramebuffer(cpnt);
}

titlebar_t* CreateTitleBar(char* title, uint32_t xPos, uint32_t yPos, uint32_t color, bool visible, component_t* parent) {
    titlebar_t* tb = malloc(sizeof(titlebar_t));

    uint32_t titleBarWidth = parent->fb->width;
    uint32_t titleBarHeight = 40;

    tb->title = title;
    tb->color = color;
    tb->visible = visible;
    tb->cpnt = AddComponent(titleBarWidth, titleBarHeight, xPos, yPos, parent);
    
    DrawTitleBar(tb->cpnt, tb->color);

    uint32_t btnWidth = 20;
    uint32_t btnHeight = titleBarHeight;
    uint32_t btnColor = 0xFFFFFFFF;

    button_t* minbtn = CreateButton(btnWidth, btnHeight, 0, 0, 0x6000FF00, btnColor, tb->cpnt);
/*     button_t* resizebtn = CreateButton((buttonparam_t){ .width = btnWidth, .height = btnHeight, .bkgColor = 0xFF00FF00, .color = btnColor, .parent = tb->cpnt });
    button_t* closebtn = CreateButton(); */

    return tb;
}

void DrawButton(component_t* cpnt, uint32_t bkgColor, uint32_t color) {
    /* color: icon color */
    fillRect(cpnt->fb, cpnt->x, cpnt->y, cpnt->fb->width, cpnt->fb->height, bkgColor);

    blitComponentFramebuffer(cpnt);
}

button_t* CreateButton(uint32_t width, uint32_t height, uint32_t xPos, uint32_t yPos, uint32_t bkgColor, uint32_t color, component_t* parent) {
    button_t* btn = malloc(sizeof(button_t));

    btn->bkgColor = bkgColor;
    btn->color = color;
    // todo: event
    btn->cpnt = AddComponent(width, height, xPos, yPos, parent);

    DrawButton(btn->cpnt, btn->bkgColor, btn->color);

    return btn;
}