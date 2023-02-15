#include <kot-ui++/context.h>

namespace Ui {
    void UiContextUpdate(Component* Cpnt){

    }

    UiContext::UiContext(framebuffer_t* fb) {
        this->Fb = fb;
        this->EventBuffer = CreateEventBuffer(fb->Width, fb->Height);
        this->Cpnt = new Component(fb, UiContextUpdate, NULL, this, true);
        this->FocusCpnt = Cpnt;
    }

}