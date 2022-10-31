#include <kot-ui++/window.h>

#include <kot-graphics++/orb.h>

#include <kot/uisd/srvs/system.h>

#include <kot-ui++/component.h>
#include <kot-ui++/components/layout/layout.h>

namespace WindowUi {

    Window::Window(char* title, uint32_t width, uint32_t height, uint32_t xPos, uint32_t yPos) {
        this->wid = orb::create(width, height, xPos, yPos);

        Graphic::framebuffer_t* fb = orb::getFramebuffer(this->wid);

        this->ctxGrph = new Graphic::ContextGphc(fb->addr, fb->width, fb->height);
        this->ctxUi = new Ui::ContextUi(fb);

        auto titlebar = Ui::titlebar(title, { .backgroundColor = WIN_BGCOLOR_ONFOCUS, .foregroundColor = 0xDDDDDD });

        this->setContent(titlebar);
 
        auto wrapper = (new Ui::FlexLayout(Ui::FlexLayout::VERTICAL))->cpnt; // <-- return the component of FlexLayout

        auto box3 = Ui::box({ .width = 30, .height = 20, .color = 0xFFFF00 });
        wrapper->addChild(box3);

        this->setContent(wrapper);
        orb::show(this->wid);
    }

    void Window::setContent(Ui::Component* content) {
        Ui::Component* windowCpnt = this->ctxUi->cpnt;

        windowCpnt->addChild(content);
        windowCpnt->draw();
    }

}