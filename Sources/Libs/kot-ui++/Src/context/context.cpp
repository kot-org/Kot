#include <kot-ui++/context.h>

namespace Ui {

    ContextUi::ContextUi(Graphic::framebuffer_t* fb) {
        this->fb = fb;

        this->cpnt = new Component(fb, {
                                    .width = fb->width,
                                    .height = fb->height,
                                    .backgroundColor = WIN_BGCOLOR_ONFOCUS
                                });
    }

}