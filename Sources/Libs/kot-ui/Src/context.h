#ifndef _CONTEXT_UI_H_
#define _CONTEXT_UI_H_

#include <kot/types.h>

#include <kot-graphics/utils.h>

#if defined(__cplusplus)
extern "C" {
#endif

typedef struct ctxui_s {
    framebuffer_t* fb;
} ctxui_t;

ctxui_t* CreateContextUi(framebuffer_t* fb);

#if defined(__cplusplus)
}
#endif

#endif