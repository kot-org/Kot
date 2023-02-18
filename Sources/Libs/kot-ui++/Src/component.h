#ifndef _UIPP_COMPONENT_H_
#define _UIPP_COMPONENT_H_

#include <kot/types.h>
#include <kot/utils/vector.h>

#include <kot-graphics/orb.h>
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

    typedef void (*MouseEventHandler)(class Component* Component, bool IsHover, int64_t RelativePositionX, int64_t RelativePositionY, int64_t PositionX, int64_t PositionY, int64_t ZValue, uint64_t Status);
    typedef void (*UpdateHandler)(class Component* Component);


    class Component {
        public:
            struct ComponentStyle{
                uint64_t Width;
                uint64_t Height;

                uint64_t ExternalStyleType;
                uintptr_t ExternalStyleData;

                bool IsVisible;
            };

            Component(ComponentStyle Style, UpdateHandler HandlerUpdate, MouseEventHandler HandlerMouseEvent, uintptr_t ExternalData, class UiContext* ParentUiContex, bool IsUpdateChild);

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
            UpdateHandler UpdateFunction;
            uintptr_t ExternalData;
            Component* Parent;
            class UiContext* UiCtx;
            bool IsFramebufferUpdate;
            bool UpdateChild;

            point_t AbsolutePosition; 
            
        private:
            ComponentStyle* Style;
            uint64_t Deep;
            framebuffer_t* Framebuffer;
            vector_t* Childs;
    };

    /* components */

    enum PictureboxType {
        _TGA = 0,
        _PNG = 1,
        _JPG = 2,
    };
    enum PictureboxFit{
        FIT     = 0,
        FILL    = 1,
        CENTER  = 2,
        STRETCH = 3,
    };
    typedef struct {
        uint64_t Width;
        uint64_t Height;
        point_t Position;
        PictureboxFit Fit;
        bool IsVisible;
    } PictureboxStyle_t;
    typedef struct {
        PictureboxType Type;
        PictureboxStyle_t Style;
        Component* Cpnt;
        uintptr_t Image;
        bool IsDrawUpdate;
    } Picturebox_t;
    Picturebox_t* Picturebox(char* Path, PictureboxType Type, PictureboxStyle_t Style, class UiContext* ParentUiContex);
    void UpdateSizePicturebox(uint64_t Width, uint64_t Height);
    void UpdatePositionPicturebox(point_t Position);

    typedef struct {
        uint64_t Width;
        uint64_t Height;
        point_t Position;
        color_t BackgroundColor;
        color_t HoverColor;
        color_t ClickColor;
        bool IsVisible;
    } BoxStyle_t;
    typedef struct {
        BoxStyle_t Style;
        Component* Cpnt;
        color_t CurrentColor;
        bool IsDrawUpdate;
    } Box_t;
    Box_t* Box(BoxStyle_t Style, class UiContext* ParentUiContex);
    void UpdateSizeBox(uint64_t Width, uint64_t Height);
    void UpdatePositionBox(point_t Position);

    typedef struct {
        uint32_t BackgroundColor;
        uint32_t ForegroundColor;
        bool IsVisible;
    } TitlebarStyle_t;
    typedef struct {
        TitlebarStyle_t Style;
        Component* Cpnt;
        bool IsDrawUpdate;
    } Titlebar_t;
    Titlebar_t* Titlebar(char* Title, TitlebarStyle_t Style, class UiContext* ParentUiContex);
    void UpdateSizeTitlebar(uint64_t Width, uint64_t Height);
    void UpdatePositionTitlebar(point_t Position);

}

namespace UiLayout {
    // Layout::ComponentPosition Position;
    // Layout::ComponentDisplay Display;
    // Layout::ComponentDirection Direction;
    // Layout::ComponentAlign_t Align;
    // Layout::ComponentSpace Space;
    void CalculateLayout(Ui::Component* Parent);

    Ui::Component* Flexbox(Ui::Component::ComponentStyle Style, class Ui::UiContext* ParentUiContex);
    Ui::Component* Gridbox(Ui::Component::ComponentStyle Style, class Ui::UiContext* ParentUiContex);

}

#endif