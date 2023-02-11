#ifndef _UIPP_COMPONENT_H_
#define _UIPP_COMPONENT_H_

#include <kot/types.h>
#include <kot/utils/vector.h>

#include <kot-graphics/utils.h>

#include <kot-ui++/context.h>


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
                Layout::ComponentPosition Position;
                Layout::ComponentDisplay Display;
                Layout::ComponentDirection Direction;
                Layout::ComponentAlign_t Align;
                Layout::ComponentSpace Space;

                uint32_t Width;
                uint32_t Height;
                uint16_t FontSize;
                uint16_t BorderRadius;

                uint32_t BackgroundColor;
                uint32_t ForegroundColor;

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
            class UiContext* UiCtx;
            uint64_t Deep;
            framebuffer_t* Framebuffer;
            ComponentStyle* Style;
            Component* Parent;
            vector_t* Childs;
            uint32_t TotalWidthChilds;
            uint32_t TotalHeightChilds;
            uint16_t Type;
            bool ReadyToBlit;
            uint64_t XAbsolute;
            uint64_t YAbsolute;

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
        uint32_t BackgroundColor;
        uint32_t ForegroundColor;
    } TitlebarStyle;
    Component* Titlebar(char* Title, TitlebarStyle Style);

}

namespace UiLayout {

    void CalculateLayout(Ui::Component* Parent);

    Ui::Component* Flexbox(Ui::Component::ComponentStyle Style);
    Ui::Component* Gridbox(Ui::Component::ComponentStyle Style);

}

#endif