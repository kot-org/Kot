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

        enum Direction {
            HORIZONTAL, // default
            VERTICAL
        };
    
        enum ComponentLayout {
            BOX = 0, // default
            FLEX = 1,
            GRID = 2,
        };

    };

    extern vector_t* lastComponents;

    class Component {

        public:

            struct ComponentStyle {
                Layout::ComponentPosition position;
                Layout::ComponentLayout display;
                Layout::Direction direction; 

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
            void editFramebuffer(uint32_t width, uint32_t height);

            void setWidth(uint32_t width);
            void setHeight(uint32_t height);

            ComponentStyle* getStyle();
            vector_t* getChilds();
            Component* getParent();

            void update();
            void draw();
            
            // todo: remove

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

    void calculateFlex(Ui::Component* parent, Ui::Component* child);
    Ui::Component* Flexbox(Ui::Component::ComponentStyle style);

}