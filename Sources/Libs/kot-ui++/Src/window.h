#ifndef _UIPP_WINDOW_H_
#define _UIPP_WINDOW_H_

#include <kot/types.h>

#include <kot++/printf.h>

#include <kot-ui++/context.h>
#include <kot-ui++/component.h>

#include <kot-graphics/orb.h>
#include <kot-graphics/context.h>

namespace UiWindow {

    class Window {
        private:
            Ui::Titlebar_t* Titlebar;

            bool IsBorders;
            ctxg_t* BordersCtx;

            bool IsListeningEvents;
            kot_event_t WindowEvent;
            kot_thread_t WindowHandlerThread;

            kot_point_t WindowNormalPosition;
            uint64_t WindowNormalWidth;
            uint64_t WindowNormalHeight;

            bool IsCpntFocus;
            void HandlerFocus(bool IsFocus);
            void HandlerMouse(uint64_t PositionX, uint64_t PositionY, uint64_t ZValue, uint64_t Status);

        public:
            bool IsFullscreen;
            kot_window_t* Wid;
            ctxg_t* GraphicCtx;
            class Ui::UiContext* UiCtx;
            Ui::Component* Cpnt; 
            
            Window(char* title, char* Icon, uint32_t Width, uint32_t Height, uint32_t XPosition, uint32_t YPosition);

            void Hide();
            void Fullscreen();
            void Close();

            void DrawBorders(uint32_t Color);

            void Handler(enum kot_Window_Event EventType, uint64_t GP0, uint64_t GP1, uint64_t GP2, uint64_t GP3, uint64_t GP4);
    };

}

#endif