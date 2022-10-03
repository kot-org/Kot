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
        if(component->param->borderRadius)
            blitFramebufferRadius(component->parent->fb, component->fb, component->param->x, component->param->y, component->param->borderRadius);
        else
            blitFramebuffer(component->parent->fb, component->fb, component->param->x, component->param->y);
        component = component->parent;
    }
}

component_t* AddComponent(component_t* parent, componentViewParam_t param) {
    /* framebuffer */
    framebuffer_t* fb = malloc(sizeof(framebuffer_t));

    uint32_t pitch = param.width * parent->fb->btpp;

    fb->size = param.height * pitch;
    fb->addr = calloc(fb->size);
    fb->pitch = pitch;
    fb->width = param.width;
    fb->height = param.height;
    fb->bpp = parent->fb->bpp;
    fb->btpp = parent->fb->btpp;

    /* parameters */
    componentViewParam_t* cpntParam = malloc(sizeof(componentViewParam_t));

    cpntParam->width = param.width;
    cpntParam->height = param.height;
    cpntParam->fontSize = param.fontSize;
    cpntParam->borderRadius = param.borderRadius;
    cpntParam->bgColor = param.bgColor;
    cpntParam->fbColor = param.fbColor;
    cpntParam->visible = param.visible;
    cpntParam->x = param.x;
    cpntParam->y = param.y;

    /* component */
    component_t* cpnt = malloc(sizeof(component_t));

    cpnt->fb = fb;
    cpnt->param = cpntParam;
    cpnt->parent = parent;
    
    if(!parent->childs)
        parent->childs = vector_create();
    vector_push(parent->childs, cpnt);

    return cpnt;
}

void EditComponent(component_t* cpnt, componentViewParam_t param) {
    cpnt->fb->width = param.width;
    cpnt->fb->height = param.height;

    cpnt->param->width = param.width;
    cpnt->param->height = param.height;
    cpnt->param->fontSize = param.fontSize;
    cpnt->param->borderRadius = param.borderRadius;
    cpnt->param->bgColor = param.bgColor;
    cpnt->param->fbColor = param.fbColor;
    cpnt->param->visible = param.visible;
    cpnt->param->x = param.x;
    cpnt->param->y = param.y;

    blitComponentFramebuffer(cpnt);
}

void RemoveComponent(component_t* cpnt) {
    for(int i = 0; i < cpnt->childs->length; i++) {
        vector_remove(cpnt->childs, i);
    }

    free(cpnt->fb);
    free(cpnt->param);
    free(cpnt);
}

component_t* GetMainParent(framebuffer_t* fb) {
    component_t* parent = malloc(sizeof(component_t));
    componentViewParam_t* param = malloc(sizeof(componentViewParam_t));

    param->width = fb->width;
    param->height = fb->height;

    parent->fb = fb;
    parent->param = param;

    return parent;
}

/* Components */

canva_t* CreateCanva(component_t* parent, componentViewParam_t param) {
    canva_t* canva = malloc(sizeof(canva_t));

    canva->cpnt = AddComponent(parent, param); 
    canva->cpnt->type = CanvaComponent;

    return canva;
}

box_t* CreateBox(component_t* parent, componentViewParam_t param) {
    box_t* box = malloc(sizeof(box_t));

    box->cpnt = AddComponent(parent, param); 
    box->cpnt->type = BoxComponent;

    return box;
}

void DrawTitleBar(component_t* cpnt) {
    /* color: text color */
    fillRect(cpnt->fb, cpnt->param->x, cpnt->param->y, cpnt->param->width, cpnt->param->height, cpnt->param->bgColor);

/*     uint8_t tmpWidthIcon = 10;

    // _ [] X buttons (test)
    drawLine(ctx, ctx->width-(height+tmpWidthIcon)/2, (height+tmpWidthIcon)/2, ctx->width-(height-tmpWidthIcon)/2, (height-tmpWidthIcon)/2, 0xFFFFFF);
    drawLine(ctx, ctx->width-(height-tmpWidthIcon)/2, (height+tmpWidthIcon)/2, ctx->width-(height+tmpWidthIcon)/2, (height-tmpWidthIcon)/2, 0xFFFFFF);
    drawRect(ctx, (ctx->width-(height+tmpWidthIcon)/2)-25, (height-tmpWidthIcon)/2, tmpWidthIcon, tmpWidthIcon, 0xFFFFFF);
    drawLine(ctx, (ctx->width-(height+tmpWidthIcon)/2)-50, height/2, (ctx->width-(height+tmpWidthIcon)/2)-40, height/2, 0xFFFFFF);  */
    blitComponentFramebuffer(cpnt);
}

titlebar_t* CreateTitleBar(char* title, component_t* parent, componentViewParam_t param) {
    titlebar_t* tb = malloc(sizeof(titlebar_t));
    
    param.width = parent->param->width;
    param.height = 40;

    tb->title = title;
    tb->cpnt = AddComponent(parent, param);
    tb->cpnt->type = TitlebarComponent;
    
    DrawTitleBar(tb->cpnt);

    uint32_t btnWidth = 20;
    uint32_t btnHeight = 20;
    uint32_t btnTextColor = 0xFFFFFFFF;

    box_t* div = CreateBox(tb->cpnt, (componentViewParam_t){ });

    button_t* minbtn = CreateButton(div->cpnt, (componentViewParam_t){ .width = btnWidth, .height = btnHeight, .bgColor = 0xFF00FF00, .fbColor = btnTextColor, .borderRadius = 10 });
    //button_t* testbtn = CreateButton(div->cpnt, (componentViewParam_t){ .width = btnWidth, .height = btnHeight, .bgColor = 0xFFFF0000, .fbColor = btnTextColor });
    /* button_t* resizebtn = CreateButton(btnWidth, btnHeight, 0, 0, 0xFFFF0000, btnColor, tb->cpnt); */
    /* button_t* closebtn = CreateButton(); */

    return tb;
}

void PrintLabel(component_t* cpnt) {
    // todo: print

    blitComponentFramebuffer(cpnt);
}

label_t* CreateLabel(char* string, component_t* parent, componentViewParam_t param) {
    label_t* lbl = malloc(sizeof(label_t));

    if(!param.fontSize)
        param.fontSize = 16;

    lbl->string = string;

    param.width = strlen(string) * param.fontSize;
    param.height = param.fontSize;

    lbl->cpnt = AddComponent(parent, param);
    lbl->cpnt->type = LabelComponent;

    PrintLabel(lbl->cpnt);

    return lbl;
}

void DrawButton(component_t* cpnt) {
    /* color: icon color */
    fillRect(cpnt->fb, 0, 0, cpnt->param->width, cpnt->param->height, cpnt->param->bgColor);

    blitComponentFramebuffer(cpnt);
}

button_t* CreateButton(component_t* parent, componentViewParam_t param) {
    button_t* btn = malloc(sizeof(button_t));

/*     char* buff[50];
    itoa(parent->type, buff, 10);
    Printlog(buff); */

    EditComponent(parent, (componentViewParam_t){ .width = 40, .height = 20 });

    // todo: event
    btn->cpnt = AddComponent(parent, param);
    btn->cpnt->type = ButtonComponent;

    DrawButton(btn->cpnt);

    return btn;
}

void DrawCheckbox(component_t* cpnt) {
    /* color: icon color */
    fillRect(cpnt->fb, cpnt->param->x, cpnt->param->y, cpnt->param->width, cpnt->param->height, cpnt->param->bgColor);

    blitComponentFramebuffer(cpnt);
}

checkbox_t* CreateCheckbox(component_t* parent, componentViewParam_t param) {
    checkbox_t* checkb = malloc(sizeof(checkbox_t));

    param.width = 30;
    param.height = 15;

    // todo: event
    checkb->cpnt = AddComponent(parent, param);
    checkb->cpnt->type = CheckboxComponent;

    DrawCheckbox(checkb->cpnt);

    return checkb;
}

void DrawTextbox(component_t* cpnt) {
    /* color: text color */
    fillRect(cpnt->fb, cpnt->param->x, cpnt->param->y, cpnt->param->width, cpnt->param->height, cpnt->param->bgColor);

    /* todo: placeholder */

    blitComponentFramebuffer(cpnt);
}

textbox_t* CreateTextbox(char* placeholder, component_t* parent, componentViewParam_t param) {
    textbox_t* textb = malloc(sizeof(textbox_t));

    // todo: event
    textb->cpnt = AddComponent(parent, param);
    textb->cpnt->type = TextboxComponent;

    DrawTextbox(textb->cpnt);

    return textb;
}

void DrawPicturebox(component_t* cpnt) {
    /* todo: draw img */

    blitComponentFramebuffer(cpnt);
}

picturebox_t* CreatePicturebox(component_t* parent, componentViewParam_t param) {
    picturebox_t* pictureb = malloc(sizeof(picturebox_t));

    // todo: img
    // todo: event
    pictureb->cpnt = AddComponent(parent, param);
    pictureb->cpnt->type = PictureboxComponent;

    DrawPicturebox(pictureb->cpnt);

    return pictureb;
}