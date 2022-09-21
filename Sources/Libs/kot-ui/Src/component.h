#ifndef _COMPONENTS_H_
#define _COMPONENTS_H_

#include <kot/types.h>

#include <kot-ui/context.h>
#include <kot-graphics/context.h>

#include <kot/utils/vector.h>

#if defined(__cplusplus)
extern "C" {
#endif

typedef struct component_s {
    ctxui_t* context;
    struct component_s* parent;
    vector_t* childs;
    uint32_t childCounter;
} component_t;

/* Components */
typedef struct {
    char* title;
    uint32_t color;
    bool visible;
    component_t* cpnt;
} titlebar_t;
typedef struct {
    char* title;
    uint32_t color;
    bool visible;
    component_t* parent;
} titlebarparam_t;

component_t* AddComponent(component_t* parent);
void RemoveComponent(component_t* cpnt);

void UpdateContext(ctxg_t* ctx);
void UpdateComponent(component_t* component);

component_t* GetMainParent(ctxui_t* ctx);

titlebar_t* CreateTitleBar(titlebarparam_t param);

void test(component_t* parent);

#if defined(__cplusplus)
}
#endif

#endif