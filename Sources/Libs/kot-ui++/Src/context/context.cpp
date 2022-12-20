#include <kot-ui++/context.h>

namespace Ui {

    UiContext::UiContext(Graphic::framebuffer_t* fb) {
        this->fb = fb;

        this->cpnt = new Component(fb);
    }

}