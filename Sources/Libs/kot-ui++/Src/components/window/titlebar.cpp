#include <kot-ui++/component.h>

namespace Ui {

    Component* titlebar(char* title, TitlebarStyle style) {
        /* todo: drag region */
        /* todo: remplacer .width = 500 par .width = 100% */
        Component* titlebar = new Component({ .width = 500, .height = 40, .backgroundColor = style.backgroundColor, .foregroundColor = style.foregroundColor });
        return titlebar;
    }

}