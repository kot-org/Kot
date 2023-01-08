#pragma once

#include <kot/types.h>

#include <kot-ui++/component.h>
#include <kot-ui++/context.h>

#include <kot-graphics/context.h>

namespace UiWindow {

    class Window {
        private:
            window_t* wid;
            ctxg_t* ctxGrph;
            Ui::UiContext* ctxUi;

        public:
            /* todo: add icon */
            Window(char* title, uint32_t Width, uint32_t Height, uint32_t XPosition, uint32_t YPosition);

            void setContent(Ui::Component* content);
    };

}