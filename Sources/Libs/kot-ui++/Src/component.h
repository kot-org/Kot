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

    typedef void (*MouseEventHandler)(class Component* Component, uint64_t RelativePositionX, uint64_t RelativePositionY, uint64_t PositionX, uint64_t PositionY, uint64_t ZValue, uint64_t Status);
    typedef void (*DrawHandler)(class Component* Component);


    class Component {
        public:
            struct ComponentStyle {
                Layout::ComponentPosition Position;
                Layout::ComponentDisplay Display;
                Layout::ComponentDirection Direction;
                Layout::ComponentAlign_t Align;
                Layout::ComponentSpace Space;

                uint32_t Width;
                uint32_t Height;

                uint32_t x;
                uint32_t y;

                uint64_t ExternalStyleType;
                uintptr_t ExternalStyleData;
            };

            /* todo: changer cette struct et mettre une struct pour charque component */

            Component(ComponentStyle Style, DrawHandler HandlerDraw, MouseEventHandler HandlerMouseEvent, uintptr_t ExternalData, class UiContext* ParentUiContex);
            /* for ui context */
            Component(framebuffer_t* fb, MouseEventHandler HandlerMouseEvent, class UiContext* ParentUiContex);

            /* Component Framebuffer */
            void CreateFramebuffer(uint32_t Width, uint32_t Height);
            void UpdateFramebuffer(uint32_t Width, uint32_t Height);

            framebuffer_t* GetFramebuffer();
            ComponentStyle* GetStyle();
            vector_t* GetChilds();
            uint32_t GetTotalWidthChilds();
            uint32_t GetTotalHeightChilds();

            void Update();
            
            // todo: remove

            void AddChild(Component* Child);

            MouseEventHandler MouseEvent;
            DrawHandler Draw;
            uintptr_t ExternalData;
            Component* Parent;
            
        private:
            class UiContext* UiCtx;
            ComponentStyle* Style;
            uint64_t Deep;
            framebuffer_t* Framebuffer;
            vector_t* Childs;
            uint32_t TotalWidthChilds;
            uint32_t TotalHeightChilds;
            uint16_t Type;
            uint64_t XAbsolute;
            uint64_t YAbsolute;

    };

    /* components */

    enum PictureboxType {
        _TGA = 0,
        _PNG = 1,
        _JPG = 2,
    };
    typedef struct {
        uint16_t Width;
        uint16_t Height;
    } PictureboxStyle_t;
    typedef struct {
        PictureboxType Type;
        PictureboxStyle_t Style;
        Component* Cpnt;
        uintptr_t Image;
    } Picturebox_t;
    Picturebox_t* Picturebox(char* Path, PictureboxType Type, PictureboxStyle_t Style, class UiContext* ParentUiContex);
    void UpdateSizePicturebox(uint64_t Width, uint64_t Height);
    void UpdatePositionPicturebox(uint64_t X, uint64_t Y);

    typedef struct {
        uint32_t Width;
        uint32_t Height;
        uint32_t Color;
    } BoxStyle_t;
    typedef struct {
        BoxStyle_t Style;
        Component* Cpnt;
    } Box_t;
    Box_t* Box(BoxStyle_t Style, class UiContext* ParentUiContex);
    void UpdateSizeBox(uint64_t Width, uint64_t Height);
    void UpdatePositionBox(uint64_t X, uint64_t Y);

    typedef struct {
        uint32_t BackgroundColor;
        uint32_t ForegroundColor;
    } TitlebarStyle_t;
    typedef struct {
        TitlebarStyle_t Style;
        Component* Cpnt;
    } Titlebar_t;
    Titlebar_t* Titlebar(char* Title, TitlebarStyle_t Style, class UiContext* ParentUiContex);
    void UpdateSizeTitlebar(uint64_t Width, uint64_t Height);
    void UpdatePositionTitlebar(uint64_t X, uint64_t Y);

}

namespace UiLayout {

    void CalculateLayout(Ui::Component* Parent);

    Ui::Component* Flexbox(Ui::Component::ComponentStyle Style, class Ui::UiContext* ParentUiContex);
    Ui::Component* Gridbox(Ui::Component::ComponentStyle Style, class Ui::UiContext* ParentUiContex);

}

#endif