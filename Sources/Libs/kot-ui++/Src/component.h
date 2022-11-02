#pragma once

#include <kot/types.h>

#include <kot-graphics++/utils.h>

#include <kot/utils/vector.h>

using namespace Graphic;

namespace Ui {

    struct Layout {

        enum ComponentPosition {
            RELATIVE, // default
            ABSOLUTE,
        };

        enum FlexDirection {
            HORIZONTAL,
            VERTICAL
        };
    
        enum ComponentLayout {
            BOX = 0, // default
            FLEX = 1,
            GRID = 2,
        };
    };

    class Component {

        public:

            struct ComponentStyle {
                Layout::ComponentPosition position = Layout::RELATIVE;
                Layout::ComponentLayout layout = Layout::BOX;

                uint32_t width;
                uint32_t height;
                uint16_t fontSize;
                uint16_t borderRadius;

                uint32_t backgroundColor;
                uint32_t foregroundColor;

                uint32_t x;
                uint32_t y;
            };

            Component(ComponentStyle style);
            Component(framebuffer_t* fb, ComponentStyle style);

            ComponentStyle* getStyle();
            vector_t* getChilds();

            void draw();
            
            void edit();
            // todo: remove, edit

            void addChild(Component* child);
            
        private:
            framebuffer_t* fb;
            ComponentStyle* style;
            Component* parent;
            vector_t* childs;
            uint16_t type;

    };

    /* components */

    typedef struct {
        uint32_t width;
        uint32_t height;
        uint32_t color;
    } BoxStyle;
    Component* box(BoxStyle style);

    typedef struct {
        uint32_t backgroundColor;
        uint32_t foregroundColor;
    } TitlebarStyle;
    Component* titlebar(char* title, TitlebarStyle style);

}

namespace UiLayout {

    void calculatePosition(Ui::Component* parent, Ui::Component* child);

    void calculateFlex(Ui::Component* parent, vector_t* childs, Ui::Component* child);
    Ui::Component* Flexbox(Ui::Component::ComponentStyle style, Ui::Layout::FlexDirection direction);

}