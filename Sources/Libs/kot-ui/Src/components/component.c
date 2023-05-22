#include <kot-ui/component.h>

#include <kot/sys.h>

/* void UpdateContext(ContextUi_t* ctx) {

}

void UpdateComponent(component_t* component) {
    
} */

void blitComponentFramebuffer(component_t* component) {
    while(component->parent != NULL) {
        if(component->param->borderRadius)
            BlitFramebufferRadius(component->parent->fb, component->fb, component->param->x, component->param->y, component->param->borderRadius);
        else
            BlitFramebuffer(component->parent->fb, component->fb, component->param->x, component->param->y);
        component = component->parent;
    }
}

component_t* AddComponent(component_t* parent, componentViewParam_t param) {
    /* framebuffer */
    kot_framebuffer_t* fb = malloc(sizeof(kot_framebuffer_t));

    uint32_t Pitch = param.Width * parent->fb->Btpp;

    fb->Size = param.Height * Pitch;
    fb->Buffer = calloc(fb->Size);
    fb->Pitch = Pitch;
    fb->Width = param.Width;
    fb->Height = param.Height;
    fb->Bpp = parent->fb->Bpp;
    fb->Btpp = parent->fb->Btpp;

    /* parameters */
    componentViewParam_t* cpntParam = malloc(sizeof(componentViewParam_t));

    cpntParam->Width = param.Width;
    cpntParam->Height = param.Height;
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
        parent->childs = kot_vector_create();
    kot_vector_push(parent->childs, cpnt);

    return cpnt;
}

void EditComponent(component_t* cpnt, componentViewParam_t param) {
    cpnt->fb->Width = param.Width;
    cpnt->fb->Height = param.Height;
    cpnt->fb->Pitch = cpnt->fb->Btpp * param.Width;

    cpnt->param->Width = param.Width;
    cpnt->param->Height = param.Height;
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

component_t* GetMainParent(kot_framebuffer_t* fb) {
    component_t* parent = malloc(sizeof(component_t));
    componentViewParam_t* param = malloc(sizeof(componentViewParam_t));

    param->Width = fb->Width;
    param->Height = fb->Height;

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

void DrawBox(component_t* cpnt) {
    FillRect(cpnt->fb, cpnt->param->x, cpnt->param->y, cpnt->param->Width, cpnt->param->Height, cpnt->param->bgColor);

    blitComponentFramebuffer(cpnt);
}

box_t* CreateBox(component_t* parent, componentViewParam_t param) {
    box_t* box = malloc(sizeof(box_t));

    if(!param.bgColor)
        param.bgColor = parent->param->bgColor; // test

    box->cpnt = AddComponent(parent, param);
    box->cpnt->type = BoxComponent;

    DrawBox(box->cpnt);

    return box;
}

void DrawTitleBar(component_t* cpnt) {
    FillRect(cpnt->fb, cpnt->param->x, cpnt->param->y, cpnt->param->Width, cpnt->param->Height, cpnt->param->bgColor);

/*     uint8_t tmpWidthIcon = 10;

    // _ [] X buttons (test)
    DrawLine(ctx, ctx->Width-(Height+tmpWidthIcon)/2, (Height+tmpWidthIcon)/2, ctx->Width-(Height-tmpWidthIcon)/2, (Height-tmpWidthIcon)/2, 0xFFFFFF);
    DrawLine(ctx, ctx->Width-(Height-tmpWidthIcon)/2, (Height+tmpWidthIcon)/2, ctx->Width-(Height+tmpWidthIcon)/2, (Height-tmpWidthIcon)/2, 0xFFFFFF);
    DrawRect(ctx, (ctx->Width-(Height+tmpWidthIcon)/2)-25, (Height-tmpWidthIcon)/2, tmpWidthIcon, tmpWidthIcon, 0xFFFFFF);
    DrawLine(ctx, (ctx->Width-(Height+tmpWidthIcon)/2)-50, Height/2, (ctx->Width-(Height+tmpWidthIcon)/2)-40, Height/2, 0xFFFFFF);  */
    blitComponentFramebuffer(cpnt);
}

titlebar_t* CreateTitleBar(char* title, component_t* parent, componentViewParam_t param) {
    titlebar_t* tb = malloc(sizeof(titlebar_t));
    
    param.Width = parent->param->Width;
    param.Height = 40;

    tb->title = title;
    tb->cpnt = AddComponent(parent, param);
    tb->cpnt->type = TitlebarComponent;
    
    DrawTitleBar(tb->cpnt);

    uint32_t btnWidth = 20;
    uint32_t btnHeight = 20;
    uint32_t btnTextColor = 0xFFFFFFFF;

    box_t* div = CreateBox(tb->cpnt, (componentViewParam_t){ .Width = 100, .Height = 20 });

    button_t* minbtn = CreateButton(div->cpnt, (componentViewParam_t){ .Width = btnWidth, .Height = btnHeight, .bgColor = 0xFF00FF00, .fbColor = btnTextColor, .borderRadius = 10 });
    button_t* testbtn = CreateButton(div->cpnt, (componentViewParam_t){ .Width = btnWidth, .Height = btnHeight, .bgColor = 0xFFFF0000, .fbColor = btnTextColor });
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

    param.Width = strlen(string) * param.fontSize;
    param.Height = param.fontSize;

    lbl->cpnt = AddComponent(parent, param);
    lbl->cpnt->type = LabelComponent;

    PrintLabel(lbl->cpnt);

    return lbl;
}

void DrawButton(component_t* cpnt) {
    /* color: icon color */
    FillRect(cpnt->fb, 0, 0, cpnt->param->Width, cpnt->param->Height, cpnt->param->bgColor);

    blitComponentFramebuffer(cpnt);
}

button_t* CreateButton(component_t* parent, componentViewParam_t param) {
    button_t* btn = malloc(sizeof(button_t));

/*     char* buff[50];
    itoa(parent->type, buff, 10);
    kot_Printlog(buff); */

    uint32_t newWidth = parent->fb->Width + param.Width;
    
    //EditComponent(parent, (componentViewParam_t){ .Width = newWidth, .Height = 20 });

    // todo: event
    btn->cpnt = AddComponent(parent, param);
    btn->cpnt->type = ButtonComponent;

    DrawButton(btn->cpnt);

    return btn;
}

void DrawCheckbox(component_t* cpnt) {
    /* color: icon color */
    FillRect(cpnt->fb, cpnt->param->x, cpnt->param->y, cpnt->param->Width, cpnt->param->Height, cpnt->param->bgColor);

    blitComponentFramebuffer(cpnt);
}

checkbox_t* CreateCheckbox(component_t* parent, componentViewParam_t param) {
    checkbox_t* checkb = malloc(sizeof(checkbox_t));

    param.Width = 30;
    param.Height = 15;

    // todo: event
    checkb->cpnt = AddComponent(parent, param);
    checkb->cpnt->type = CheckboxComponent;

    DrawCheckbox(checkb->cpnt);

    return checkb;
}

void DrawTextbox(component_t* cpnt) {
    /* color: text color */
    FillRect(cpnt->fb, cpnt->param->x, cpnt->param->y, cpnt->param->Width, cpnt->param->Height, cpnt->param->bgColor);

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