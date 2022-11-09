#include <kot-ui++/context.h>

namespace Ui {

    vector_t* lastComponents;

    UiContext::UiContext(Graphic::framebuffer_t* fb) {
        this->fb = fb;

        this->cpnt = new Component(fb);
        lastComponents = vector_create();
    }

}