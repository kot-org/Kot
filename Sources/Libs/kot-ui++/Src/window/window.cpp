#include <kot-ui++/window.h>

#include <kot-graphics++/orb.h>

#include <kot/uisd/srvs/system.h>

#include <kot-ui++/component.h>

namespace UiWindow {

    Window::Window(char* title, uint32_t width, uint32_t height, uint32_t xPos, uint32_t yPos) {
        this->wid = orb::create(width, height, xPos, yPos);

        Graphic::framebuffer_t* fb = orb::getFramebuffer(this->wid);

        this->ctxGrph = new Graphic::ContextGphc(fb->addr, fb->width, fb->height);
        this->ctxUi = new Ui::UiContext(fb);

        auto titlebar = Ui::titlebar(title, { .backgroundColor = WIN_BGCOLOR_ONFOCUS, .foregroundColor = 0xDDDDDD });
        this->setContent(titlebar);
 
/*         auto wrapper = Ui::box({ .width = this->ctxUi->fb->width, .height = this->ctxUi->fb->height - titlebar->getStyle()->height, .color = WIN_BGCOLOR_ONFOCUS });

        auto flexbox = UiLayout::Flexbox({}, Ui::Layout::HORIZONTAL);

        auto box = Ui::box({ .width = 20, .height = 20, .color = 0xFFFF00 });
        flexbox->addChild(box);

        wrapper->addChild(flexbox);

        this->setContent(wrapper); */

        orb::show(this->wid);
    }

    void Window::update() {
        for(int i = 0; i < Ui::lastComponents->length; i++) {
            Ui::Component* component = (Ui::Component*) vector_get(Ui::lastComponents, i);
            Printlog("last child");
            component->update();
        }
    }

    void Window::setContent(Ui::Component* content) {
        Ui::Component* windowCpnt = this->ctxUi->cpnt;

        windowCpnt->addChild(content);

        this->update();

        //windowCpnt->draw();
    }

}