#ifndef _UIPP_COMPONENT_H_
#define _UIPP_COMPONENT_H_

#include <kot/types.h>
#include <kot/utils/vector.h>

#include <kot-graphics/orb.h>
#include <kot-graphics/utils.h>

#include <kot-ui++/context.h>



namespace Ui {

    struct Layout {
        /* Space */
        enum ComponentHorizontalSpace {
            FILLHORIZONTAL = 0,
            BETWEENHORIZONTAL = 1,
            AROUNDHORIZONTAL = 2,
            LEFT    = 3,
            CENTER  = 4,
            RIGHT   = 5,
        };

        enum ComponentVerticalSpace {
            FILLVERTICAL = 0,
            BETWEENVERTICAL = 1,
            AROUNDVERTICAL = 2,
            TOP     = 3,
            MIDDLE  = 4,
            BOTTOM  = 5,
        };

        struct ComponentAlign_t {
            ComponentHorizontalSpace x;
            ComponentVerticalSpace y;
        };

    };

    enum ButtonEventType{
        ButtonEventTypeHover = 0,
        ButtonEventTypeLeftClick = 1,
        ButtonEventTypeRightClick = 2,
        ButtonEventTypeMiddleClick = 3,
        ButtonEventTypeUnfocus = 4,
    };

    typedef void (*MouseEventHandler)(class Component* Component, bool IsHover, int64_t RelativePositionX, int64_t RelativePositionY, int64_t PositionX, int64_t PositionY, int64_t ZValue, uint64_t Status);
    typedef void (*UpdateHandler)(class Component* Component);
    typedef void (*ButtonHandler)(struct Button_t* Button, ButtonEventType EventType);


    class Component {
        public:
            struct ComponentStyle{
                uint64_t Width;
                uint64_t Height;
                point_t Position;

                uint64_t ExternalStyleType;
                uintptr_t ExternalStyleData;

                bool IsVisible;

                bool IsHorizontalOverflow;
                bool IsVerticalOverflow;
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

            Component* VerticalOverflow;
            Component* HorizontalOverflow;

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
        PICTUREFIT     = 0,
        PICTUREFILL    = 1,
        PICTURECENTER  = 2,
        PICTURESTRETCH = 3,
    };
    typedef struct {
        uint64_t Width;
        uint64_t Height;
        point_t Position;
        PictureboxFit Fit;
        bool IsVisible;
    } PictureboxStyle_t;
    struct Picturebox_t{
        PictureboxType Type;
        PictureboxStyle_t Style;
        Component* Cpnt;
        uintptr_t Image;
        bool IsDrawUpdate;
    };
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
    struct Box_t{
        BoxStyle_t Style;
        Component* Cpnt;
        color_t CurrentColor;
        bool IsDrawUpdate;
    };
    Box_t* Box(BoxStyle_t Style, class UiContext* ParentUiContex);
    void UpdateSizeBox(uint64_t Width, uint64_t Height);
    void UpdatePositionBox(point_t Position);

    typedef struct {
        uint64_t Width;
        uint64_t Height;
        point_t Position;
        Layout::ComponentAlign_t Align;
        bool IsVisible;
    } FlexboxStyle_t;
    struct Flexbox_t{
        FlexboxStyle_t Style;
        Component* Cpnt;
        color_t CurrentColor;
    };
    Flexbox_t* Flexbox(FlexboxStyle_t Style, class UiContext* ParentUiContex);
    void UpdateSizeFlexbox(uint64_t Width, uint64_t Height);
    void UpdatePositionFlexbox(point_t Position);

    typedef struct {
        uint64_t Width;
        uint64_t Height;
        uint64_t CaseWidth;
        uint64_t CaseHeight;
        point_t Position;
        bool IsVisible;
    } GridboxStyle_t;
    struct Gridbox_t{
        GridboxStyle_t Style;
        Component* Cpnt;
    };
    Gridbox_t* Gridbox(GridboxStyle_t Style, class UiContext* ParentUiContex);
    void UpdateSizeGridbox(uint64_t Width, uint64_t Height);
    void UpdatePositionGridbox(point_t Position);


    typedef struct {
        uint64_t Width;
        uint64_t Height;
        point_t Position;
        color_t BackgroundColor;
        color_t HoverColor;
        color_t ClickColor;
        ButtonHandler OnClick;
        bool IsVisible;
    } ButtonStyle_t;
    struct Button_t{
        ButtonStyle_t Style;
        Component* Cpnt;
        color_t CurrentColor;
        bool IsDrawUpdate;
    };
    Button_t* Button(ButtonStyle_t Style, class UiContext* ParentUiContex);
    void UpdateSizeButton(uint64_t Width, uint64_t Height);
    void UpdatePositionButton(point_t Position);

    typedef struct {
        uint32_t BackgroundColor;
        uint32_t ForegroundColor;
        bool IsVisible;
    } TitlebarStyle_t;
    struct Titlebar_t{
        TitlebarStyle_t Style;
        Component* Cpnt;
        bool IsDrawUpdate;
    };
    Titlebar_t* Titlebar(char* Title, TitlebarStyle_t Style, class UiContext* ParentUiContex);
    void UpdateSizeTitlebar(uint64_t Width, uint64_t Height);
    void UpdatePositionTitlebar(point_t Position);

}

#endif