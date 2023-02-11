#ifndef _UIPP_CONTEXT_H_
#define _UIPP_CONTEXT_H_

#include <kot/types.h>

#include <kot-graphics/utils.h>
#include <kot-graphics/event.h>

#include <kot-ui++/component.h>

#define WIN_BGCOLOR_ONFOCUS 0x181818
#define WIN_BGCOLOR_ONBLUR 0x2B2B2B

#define WIN_BDCOLOR_ONFOCUS 0x2D9CDB
#define WIN_BDCOLOR_ONBLUR 0x2B2B2B

namespace Ui {

    class UiContext {
        private:
        public:
            framebuffer_t* Fb;
            class Component* Cpnt;
            graphiceventbuffer_t* EventBuffer;

            UiContext(framebuffer_t* Fb);

    };
}

#endif