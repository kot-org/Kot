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

        enum ComponentDirection {
            HORIZONTAL, // default
            VERTICAL
        };

        /* Alignment */
        enum ComponentHorizontalAlign {
            LEFT    = 0,
            CENTER  = 1,
            RIGHT   = 2,
        };

        enum ComponentVerticalAlign {
            TOP     = 0,
            MIDDLE  = 1,
            BOTTOM  = 2,
        };
    
        /* Display */
        enum ComponentDisplay {
            BOX     = 0, // default
            FLEX    = 1,
            GRID    = 2,
        };

    };

    extern vector_t* lastComponents;

    class Component {

        public:

            struct ComponentStyle {
                Layout::ComponentPosition position;
                Layout::ComponentDisplay display;
                Layout::ComponentDirection direction;
                Layout::ComponentHorizontalAlign horizontalAlign;
                Layout::ComponentVerticalAlign verticalAlign;

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
            /* for ui context */
            Component(framebuffer_t* fb);

            /* Component Framebuffer */
            void createFramebuffer(uint32_t width, uint32_t height);

            ComponentStyle* getStyle();
            vector_t* getChilds();
            uint32_t getTotalWidthChilds();
            uint32_t getTotalHeightChilds();

            void update();
            void draw();
            
            // todo: remove

            void addChild(Component* child);
            
        private:
            framebuffer_t* fb;
            ComponentStyle* style;
            Component* parent;
            vector_t* childs;
            uint32_t totalWidthChilds;
            uint32_t totalHeightChilds;
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

    void calculateLayout(Ui::Component* parent);

    Ui::Component* Flexbox(Ui::Component::ComponentStyle style);

}