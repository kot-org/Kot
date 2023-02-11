#include <kot-ui++/component.h>

namespace Ui {

    Component* Titlebar(char* Title, TitlebarStyle Style) {
        /* todo: drag region */
        /* todo: replace Width: 500 -> 100% */
        Component* titlebar = UiLayout::Flexbox({ .direction = Layout::HORIZONTAL, .space = Layout::BETWEEN, .Width = 500, .backgroundColor = Style.backgroundColor, .foregroundColor = Style.foregroundColor });

        auto windowInfo = Ui::Box({ .Width = titlebar->GetStyle()->Width/2, .Height = 40, .color = 0xFF0000 });

        auto iconBox = Ui::Box({ .Width = 20, .Height = 20, .color = 0xFFFF00 });
        windowInfo->AddChild(iconBox); 
        auto titleBox = Ui::Box({ .Width = 40, .Height = 25, .color = 0xFF00FF });
        windowInfo->AddChild(titleBox); 
        
        titlebar->AddChild(windowInfo);

        auto buttons = Ui::Box({ .Width = 60, .color = 0x00FF00 });

        /* auto box2box = Ui::box({ .Width = 10, .Height = 10, .color = 0xFFFF00 });
        buttons->addChild(box2box);  */

        titlebar->AddChild(buttons);

        return titlebar;
    }

}