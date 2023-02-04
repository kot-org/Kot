#include <kot-ui++/component.h>

namespace Ui {

    Component* titlebar(char* title, TitlebarStyle style) {
        /* todo: drag region */
        /* todo: replace Width: 500 -> 100% */
        Component* titlebar = UiLayout::Flexbox({ .direction = Layout::HORIZONTAL, .space = Layout::BETWEEN, .Width = 500, .backgroundColor = style.backgroundColor, .foregroundColor = style.foregroundColor });

        auto windowInfo = Ui::box({ .Width = titlebar->GetStyle()->Width/2, .Height = 40, .color = 0xFF0000 });

        auto iconBox = Ui::box({ .Width = 20, .Height = 20, .color = 0xFFFF00 });
        windowInfo->addChild(iconBox); 
        auto titleBox = Ui::box({ .Width = 40, .Height = 25, .color = 0xFF00FF });
        windowInfo->addChild(titleBox); 
        
        titlebar->addChild(windowInfo);

        auto buttons = Ui::box({ .Width = 60, .color = 0x00FF00 });

        /* auto box2box = Ui::box({ .Width = 10, .Height = 10, .color = 0xFFFF00 });
        buttons->addChild(box2box);  */

        titlebar->addChild(buttons);

        return titlebar;
    }

}