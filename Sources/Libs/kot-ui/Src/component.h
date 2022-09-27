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
    ButtonComponent = 0,
    CheckboxComponent = 1,
    TextboxComponent = 2,
    PictureboxComponent = 3,
};

typedef struct component_s {
    framebuffer_t* fb;
    ctxui_t* ctx;
    struct component_s* parent;
    vector_t* childs;
    uint32_t x;
    uint32_t y;
} component_t;

typedef struct {
    
} grid_t;

/* Components */
typedef struct {
    component_t* cpnt;
} canva_t;

typedef struct {
    char* title;
    // todo: icon
    uint32_t color;
    bool visible;
    component_t* cpnt;
} titlebar_t;

typedef struct {
    uint32_t bkgColor;
    uint32_t color;
    // todo: event
    component_t* cpnt;
} button_t;

component_t* AddComponent(uint32_t width, uint32_t height, uint32_t xPos, uint32_t yPos, component_t* parent);
void RemoveComponent(component_t* cpnt);

void UpdateContext(ctxui_t* ctx);
void UpdateComponent(component_t* component);

component_t* GetMainParent(framebuffer_t* fb);

/* Components */

canva_t* CreateCanva(uint32_t width, uint32_t height, uint32_t xPos, uint32_t yPos, component_t* parent);

void DrawTitleBar(component_t* cpnt, uint32_t color);
titlebar_t* CreateTitleBar(char* title, uint32_t xPos, uint32_t yPos, uint32_t color, bool visible, component_t* parent);

void DrawButton(component_t* cpnt, uint32_t bkgColor, uint32_t color);
button_t* CreateButton(uint32_t width, uint32_t height, uint32_t xPos, uint32_t yPos, uint32_t bkgColor, uint32_t color, component_t* parent);

#if defined(__cplusplus)
}
#endif

#endif