#ifndef _CONTEXT_UI_H_
#define _CONTEXT_UI_H_

#include <kot/types.h>

#include <kot-graphics/context.h>

#if defined(__cplusplus)
extern "C" {
#endif

typedef struct ctxui_s {
    ctxg_t* ctxg;
} ctxui_t;

ctxui_t* CreateContextUi(ctxg_t* ctxGraphic);

#if defined(__cplusplus)
}
#endif

#endif