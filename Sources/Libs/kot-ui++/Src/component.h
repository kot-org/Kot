#ifndef _UIPP_COMPONENT_H_
#define _UIPP_COMPONENT_H_

#include <kot/types.h>
#include <kot/utils/vector.h>

#include <kot-graphics/utils.h>


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
    
        /* Display */
        enum ComponentDisplay {
            BOX     = 0, // default
            FLEX    = 1,
            GRID    = 2,
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

        struct ComponentAlign_t {
            ComponentHorizontalAlign x;
            ComponentVerticalAlign y;
        };

        /* Space */
        enum ComponentSpace {
            BETWEEN = 1,
            AROUND = 2,
        };

    };



    class Component {
        public:
            struct ComponentStyle {
                // todo: modifier ca
                Layout::ComponentPosition position;
                Layout::ComponentDisplay display;
                Layout::ComponentDirection direction;
                Layout::ComponentAlign_t align;
                Layout::ComponentSpace space;

                uint32_t Width;
                uint32_t Height;
                uint16_t fontSize;
                uint16_t borderRadius;

                uint32_t backgroundColor;
                uint32_t foregroundColor;

                uint32_t x;
                uint32_t y;
            };

            /* todo: changer cette struct et mettre une struct pour charque component */

            Component(ComponentStyle Style);
            /* for ui context */
            Component(framebuffer_t* fb);

            /* Component Framebuffer */
            void CreateFramebuffer(uint32_t Width, uint32_t Height);
            void UpdateFramebuffer(uint32_t Width, uint32_t Height);

            framebuffer_t* GetFramebuffer();
            ComponentStyle* GetStyle();
            vector_t* GetChilds();
            uint32_t GetTotalWidthChilds();
            uint32_t GetTotalHeightChilds();

            void Update();
            void Draw();
            
            // todo: remove

            void AddChild(Component* child);

            void MouseEvent(uint64_t RelativePositionX, uint64_t RelativePositionY, uint64_t PositionX, uint64_t PositionY, uint64_t ZValue, uint64_t Status);
            
        private:
            Component* MainParent;
            framebuffer_t* Framebuffer;
            ComponentStyle* Style;
            Component* Parent;
            vector_t* Childs;
            uint32_t TotalWidthChilds;
            uint32_t TotalHeightChilds;
            uint16_t Type;
            bool ReadyToBlit;

    };

    /* components */

    enum ImageType {
        _TGA = 0,
        _PNG = 1,
        _JPG = 2,
    };
    typedef struct {
        uint16_t Width;
        uint16_t Height;
    } ImageStyle;
    Component* Picturebox(char* Path, ImageType Type, ImageStyle Style);

    typedef struct {
        uint32_t Width;
        uint32_t Height;
        uint32_t color;
    } BoxStyle;
    Component* Box(BoxStyle Style);

    typedef struct {
        uint32_t backgroundColor;
        uint32_t foregroundColor;
    } TitlebarStyle;
    Component* Titlebar(char* Title, TitlebarStyle Style);

}

namespace UiLayout {

    void CalculateLayout(Ui::Component* Parent);

    Ui::Component* Flexbox(Ui::Component::ComponentStyle Style);
    Ui::Component* Gridbox(Ui::Component::ComponentStyle Style);

}

#endif