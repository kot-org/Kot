#include <kot-ui++/context.h>

namespace Ui {

    UiContext::UiContext(framebuffer_t* fb) {
        this->Fb = fb;
        this->EventBuffer = CreateEventBuffer(fb->Width, fb->Height);
        this->Cpnt = new Component(fb, NULL, this);
    }

}