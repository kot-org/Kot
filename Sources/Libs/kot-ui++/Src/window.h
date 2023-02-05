#ifndef _UIPP_WINDOW_H_
#define _UIPP_WINDOW_H_

#include <kot/types.h>

#include <kot-ui++/event.h>
#include <kot-ui++/context.h>
#include <kot-ui++/component.h>

#include <kot-graphics/context.h>

namespace UiWindow {

    class Window {
        private:
            window_t* Wid;
            ctxg_t* GraphicCtx;
            Ui::UiContext* UiCtx;

            bool Borders;
            ctxg_t* BordersCtx;

            event_t WindowEvent;
            thread_t WindowHandlerThread;
            UiEvent::Event* EventData;
            graphiceventbuffer_t* EventBuffer;
            void HandlerFocus(bool IsFocus);
            void HandlerMouse(uint64_t PositionX, uint64_t PositionY, uint64_t ZValue, uint64_t Status);

        public:
            /* todo: add icon */
            Window(char* title, uint32_t Width, uint32_t Height, uint32_t XPosition, uint32_t YPosition);

            void DrawBorders(uint32_t Color);

            void SetContent(Ui::Component* content);     
            void Handler(enum Window_Event EventType, uint64_t GP0, uint64_t GP1, uint64_t GP2, uint64_t GP3, uint64_t GP4);
    };

}

#endif