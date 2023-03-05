#ifndef _UIPP_CONTEXT_H_
#define _UIPP_CONTEXT_H_

#include <kot/types.h>

#include <kot-graphics/utils.h>
#include <kot-graphics/event.h>

#include <kot-ui++/component.h>

#define WIN_BGCOLOR_ONFOCUS 0x2B2B2B // BG : background
#define WIN_BGCOLOR_ONBLUR 0x3B3B3B // BG : background

#define WIN_BDCOLOR_ONFOCUS 0x3B3B3B // BD : borders
#define WIN_BDCOLOR_ONBLUR 0x101010 // BD : borders

#define WIN_TBCOLOR_ONFOCUS 0x2B2B2B // TB : titlebar
#define WIN_TBCOLOR_ONBLUR 0x3B3B3B // TB : titlebar

#define WIN_DEFAULT_BKGCOLOR 0x1E1E1E // BKG : background

namespace Ui {

    class UiContext {
        private:
            thread_t ThreadRenderer;
        public:
            UiContext(framebuffer_t* Fb);
            void UpdateFramebuffer(framebuffer_t* fb);
            void UiStartRenderer();
            void UiStopRenderer();

            bool IsRendering;
            bool Renderer;
            bool IsListeningEvents;
            framebuffer_t* Fb;
            class Component* Cpnt;
            class Component* FocusCpnt;
            graphiceventbuffer_t* EventBuffer;
            graphiceventbuffer_t* EventBufferUse;
    };
}

#endif