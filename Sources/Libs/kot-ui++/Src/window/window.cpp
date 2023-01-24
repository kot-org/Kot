#include <kot-ui++/window.h>

#include <kot-graphics/orb.h>

#include <kot/uisd/srvs/system.h>

#include <kot-ui++/component.h>

namespace UiWindow {

    Window::Window(char* title, uint32_t Width, uint32_t Height, uint32_t XPosition, uint32_t YPosition) {
        this->wid = CreateWindow(NULL, Window_Type_Default);
        ResizeWindow(this->wid, Width, Height);
        WindowChangePosition(this->wid, XPosition, YPosition);

        framebuffer_t* fb = &this->wid->Framebuffer;

        this->ctxGrph = CreateGraphicContext(fb);
        this->ctxUi = new Ui::UiContext(fb);

        auto imgtest = Ui::Picturebox("kotlogo.tga", Ui::ImageType::_TGA, { .Width = 256, .Height = 256 });
        this->setContent(imgtest);

        /* auto titlebar = Ui::titlebar(title, { .backgroundColor = WIN_BGCOLOR_ONFOCUS, .foregroundColor = 0xDDDDDD });
        this->setContent(titlebar); */
 
/*         auto wrapper = Ui::box({ .Width = this->ctxUi->fb->Width, .Height = this->ctxUi->fb->Height - titlebar->getStyle()->Height, .color = WIN_BGCOLOR_ONFOCUS });

        auto flexbox = UiLayout::Flexbox({}, Ui::Layout::HORIZONTAL);

        auto box = Ui::box({ .Width = 20, .Height = 20, .color = 0xFFFF00 });
        flexbox->addChild(box);

        wrapper->addChild(flexbox);

        this->setContent(wrapper); */

        ChangeVisibilityWindow(this->wid, true);
    }

    void Window::setContent(Ui::Component* content) {
        Ui::Component* windowCpnt = this->ctxUi->cpnt;

        windowCpnt->addChild(content);
        windowCpnt->update();
    }

}