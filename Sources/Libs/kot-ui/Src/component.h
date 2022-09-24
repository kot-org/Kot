#ifndef _COMPONENTS_H_
#define _COMPONENTS_H_

#include <kot/types.h>

#include <kot-ui/context.h>
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
    ctxui_t* context;
    struct component_s* parent;
    vector_t* childs;
    uint32_t childCounter;
    uint32_t width;
    uint32_t height;
} component_t;

/* Components */
typedef struct {
    component_t* cpnt;
} canva_t;
typedef struct {
    uint32_t width;
    uint32_t height;
    component_t* parent;
} canvaparam_t;

typedef struct {
    char* title;
    // todo: icon
    uint32_t color;
    bool visible;
    component_t* cpnt;
} titlebar_t;
typedef struct {
    char* title;
    // todo: icon
    uint32_t color;
    bool visible;
    component_t* parent;
} titlebarparam_t;

component_t* AddComponent(uint32_t width, uint32_t height, component_t* parent);
void RemoveComponent(component_t* cpnt);

void UpdateContext(ctxg_t* ctx);
void UpdateComponent(component_t* component);

component_t* GetMainParent(ctxui_t* ctx);

canva_t* CreateCanva(canvaparam_t param);
titlebar_t* CreateTitleBar(titlebarparam_t param);

#if defined(__cplusplus)
}
#endif

#endif