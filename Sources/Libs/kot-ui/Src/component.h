#ifndef _COMPONENTS_H_
#define _COMPONENTS_H_

#include <kot/types.h>

#include <kot-ui/context.h>
#include <kot-graphics/utils.h>
#include <kot-graphics/context.h>

#include <kot/utils/vector.h>

#if defined(__cplusplus)
extern "C" {
#endif

enum ComponentType {
    CanvaComponent      = 0,
    BoxComponent        = 1,
    TitlebarComponent   = 2,
    LabelComponent      = 3,
    ButtonComponent     = 4,
    CheckboxComponent   = 5,
    TextboxComponent    = 6,
    PictureboxComponent = 7,
};

typedef struct {
    uint32_t width;
    uint32_t height;
    uint16_t fontSize;
    uint16_t borderRadius;
    uint32_t bgColor;
    uint32_t fbColor;
    bool visible;
    uint32_t x;
    uint32_t y;
} componentViewParam_t;

typedef struct component_s {
    framebuffer_t* fb;
    ctxui_t* ctx;
    componentViewParam_t* param;
    struct component_s* parent;
    vector_t* childs;
    uint16_t type;
} component_t;

typedef struct {
    
} grid_t;

/* Components */
typedef struct {
    component_t* cpnt;
} canva_t;

typedef struct {
    component_t* cpnt;
} box_t;

typedef struct {
    char* title;
    // todo: icon
    component_t* cpnt;
} titlebar_t;

typedef struct {
    char* string;
    component_t* cpnt;
} label_t;

typedef struct {
    // todo: event
    component_t* cpnt;
} button_t;

typedef struct {
    // todo: event
    component_t* cpnt;
} checkbox_t;

typedef struct {
    char* placeholder;
    // todo: event
    component_t* cpnt;
} textbox_t;

typedef struct {
    // todo: img
    // todo: event
    component_t* cpnt;
} picturebox_t;

void UpdateContext(ctxui_t* ctx);
void UpdateComponent(component_t* component);

void blitComponentFramebuffer(component_t* component);

component_t* AddComponent(component_t* parent, componentViewParam_t param);
void EditComponent(component_t* cpnt, componentViewParam_t param);
void RemoveComponent(component_t* cpnt);

component_t* GetMainParent(framebuffer_t* fb);

/* Components */

canva_t* CreateCanva(component_t* parent, componentViewParam_t param);
box_t* CreateBox(component_t* parent, componentViewParam_t param);

void DrawTitleBar(component_t* cpnt);
titlebar_t* CreateTitleBar(char* title, component_t* parent, componentViewParam_t param);
 
void PrintLabel(component_t* cpnt);
label_t* CreateLabel(char* string, component_t* parent, componentViewParam_t param);

void DrawButton(component_t* cpnt);
button_t* CreateButton(component_t* parent, componentViewParam_t param);

void DrawCheckbox(component_t* cpnt);
checkbox_t* CreateCheckbox(component_t* parent, componentViewParam_t param);

void DrawTextbox(component_t* cpnt);
textbox_t* CreateTextbox(char* placeholder, component_t* parent, componentViewParam_t param);

void DrawPicturebox(component_t* cpnt);
picturebox_t* CreatePicturebox(component_t* parent, componentViewParam_t param);

#if defined(__cplusplus)
}
#endif

#endif