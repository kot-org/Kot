#pragma once

#include <kot/types.h>

#include <kot-graphics++/utils.h>

#include <kot-ui++/component.h>

namespace Ui {

    class ContextUi {
        
        private:
        public:
            Graphic::framebuffer_t* fb;
            Component* cpnt;

            ContextUi(Graphic::framebuffer_t* fb);

    };
}