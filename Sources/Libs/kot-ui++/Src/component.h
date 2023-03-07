#ifndef _UIPP_COMPONENT_H_
#define _UIPP_COMPONENT_H_

#include <kot/types.h>
#include <kot/stdio.h>
#include <kot/utils/vector.h>

#include <kot-graphics/orb.h>
#include <kot-graphics/font.h>
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

        enum ComponentDirection{
            ROW     = 0,
            COLUMN  = 1,
        };
    };

    #define NO_MAXIMUM                      (0xffffffffffffffff)


    #define BUTTON_EVENT_TYPE_HOVER         (1 << 0)
    #define BUTTON_EVENT_TYPE_LEFT_CLICK    (1 << 1)
    #define BUTTON_EVENT_TYPE_RIGHT_CLICK   (1 << 2)
    #define BUTTON_EVENT_TYPE_MIDDLE_CLICK  (1 << 3)
    #define BUTTON_EVENT_TYPE_UNFOCUS       (1 << 4)

    typedef uint64_t ButtonStatus_t;

    typedef void (*MouseEventHandler)(class Component* Component, bool IsHover, int64_t RelativePositionX, int64_t RelativePositionY, int64_t PositionX, int64_t PositionY, int64_t ZValue, uint64_t Status);
    typedef void (*UpdateHandler)(class Component* Component);
    typedef void (*ButtonEvent_t)(struct Button_t* Button, ButtonStatus_t EventType);

    struct Margin{
        uint64_t Top;
        uint64_t Bottom;
        uint64_t Left;
        uint64_t Right;
    };


    enum AlignTypeX {
        LEFT = 0,
        CENTER = 1,
        RIGHT = 2,
    };

    enum AlignTypeY {
        TOP = 0,
        MIDDLE = 1,
        BOTTOM = 2,
    };

    struct align_t{
        AlignTypeX x;
        AlignTypeY y;
    };

    enum TextAlign {
        TEXTALIGNLEFT = 0,
        TEXTALIGNCENTER = 1,
        TEXTALIGNRIGHT = 2,       
    };

    struct ComponentGeneralStyle{
        uint64_t Maxwidth = NO_MAXIMUM;
        uint64_t Minwidth = 0;
        uint64_t Currentwidth = 0;
        int64_t Width = 0;

        uint64_t Maxheight = NO_MAXIMUM;
        uint64_t Minheight = 0;
        uint64_t Currentheight = 0;
        int64_t Height = 0;

        uint64_t BorderRadius = 0;

        Margin Margin{
            .Top = 0,
            .Bottom = 0,
            .Left = 0,
            .Right = 0,
        };

        align_t Align{
            .x = AlignTypeX::LEFT,
            .y = AlignTypeY::TOP,
        };

        bool AutoPosition = true;

        point_t Position{
            .x = 0,
            .y = 0,
        };

        uint64_t ZIndex = 0; // TODO

        uint64_t ExternalStyleType = 0;
        uintptr_t ExternalStyleData = 0;

        bool IsHidden = false;

        bool IsHorizontalOverflow = false;
        bool IsVerticalOverflow = false;
    };

    class Component {
        public:
            Component(ComponentGeneralStyle Style, UpdateHandler HandlerUpdate, MouseEventHandler HandlerMouseEvent, uintptr_t ExternalData, class Component* ParentCpnt, bool IsOwnFb);

            /* Component Framebuffer */
            void CreateFramebuffer(uint32_t Width, uint32_t Height);
            void UpdateFramebuffer(uint32_t Width, uint32_t Height);

            framebuffer_t* GetFramebuffer();
            ComponentGeneralStyle* GetStyle();
            vector_t* GetChilds();
            uint32_t GetTotalWidthChilds();
            uint32_t GetTotalHeightChilds();

            void Update();

            void Free();
            void ClearChilds();


            MouseEventHandler MouseEvent;
            UpdateHandler UpdateFunction;
            uintptr_t ExternalData;
            Component* Parent;
            class UiContext* UiCtx;
            bool IsFramebufferUpdate;
            bool IsRedraw;
            bool IsDrawUpdate;

            point_t DrawPosition; 
            point_t AbsolutePosition;     
            point_t FramebufferRelativePosition;

            Component* VerticalOverflow;
            Component* HorizontalOverflow;

            ComponentGeneralStyle* Style;
            uint64_t Deep;

            uint64_t Index;

            framebuffer_t* Framebuffer;
            bool OwnFb;
            
            vector_t* Childs;

            // Component.cpp private function
            void AddChild(Component* Child);
    };

    /* Layout */

    /* Flexbox */
    typedef struct {
        ComponentGeneralStyle G;
        Layout::ComponentAlign_t Align = {
            .x = Layout::LEFT,
            .y = Layout::TOP,
        };
        Layout::ComponentDirection Direction = Layout::ROW;
    } FlexboxStyle_t;

    struct Flexbox_t{
        FlexboxStyle_t Style;
        Component* Cpnt;
        color_t CurrentColor;
        void UpdateSize(uint64_t Width, uint64_t Height);
        void UpdatePosition(point_t Position);
    };

    Flexbox_t* Flexbox(FlexboxStyle_t Style, class Component* ParentCpnt);


    /* Gridbox */
    typedef struct {
        ComponentGeneralStyle G;
        int64_t CaseWidth = 0;
        int64_t CaseHeight = 0;
        uint64_t SpaceBetweenCaseHorizontal = 0;
        uint64_t SpaceBetweenCaseVertical = 0;
    } GridboxStyle_t;

    struct Gridbox_t{
        GridboxStyle_t Style;
        Component* Cpnt;
        void UpdateSize(uint64_t Width, uint64_t Height);
        void UpdatePosition(point_t Position);
    };

    Gridbox_t* Gridbox(GridboxStyle_t Style, class Component* ParentCpnt); 

    /* Components */

    /* Picturebox */
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
        ComponentGeneralStyle G;
        PictureboxFit Fit = PICTUREFIT;
        bool Transparency = false;
    } PictureboxStyle_t;

    struct Picturebox_t{
        PictureboxType Type;
        PictureboxStyle_t Style;
        Component* Cpnt;
        uintptr_t Image;
        void UpdateSize(uint64_t Width, uint64_t Height);
        void UpdatePosition(point_t Position);
    };
    Picturebox_t* Picturebox(char* Path, PictureboxType Type, PictureboxStyle_t Style, class Component* ParentCpnt);


    /* Box */
    typedef struct {
        ComponentGeneralStyle G;
        color_t BackgroundColor = 0;
        color_t HoverColor = BackgroundColor;
        color_t ClickColor = HoverColor;
    } BoxStyle_t;

    struct Box_t{
        BoxStyle_t Style;
        Component* Cpnt;
        color_t CurrentColor;
        void UpdateSize(uint64_t Width, uint64_t Height);
        void UpdatePosition(point_t Position);
    };

    Box_t* Box(BoxStyle_t Style, class Component* ParentCpnt);

    
    /* Button */
    typedef struct {
        ComponentGeneralStyle G;
        color_t BackgroundColor = 0;
        color_t HoverColor = BackgroundColor;
        color_t ClickColor = HoverColor;
        uint64_t ExternalData = 0;
    } ButtonStyle_t;

    struct Button_t {
        ButtonStyle_t Style;
        Component* Cpnt;
        color_t CurrentColor;
        ButtonEvent_t Event;
        
        void UpdateSize(uint64_t Width, uint64_t Height);
        void UpdatePosition(point_t Position);
    };

    Button_t* Button(ButtonEvent_t Event, ButtonStyle_t Style, class Component* ParentCpnt);


    /* Label */
    typedef struct {
        ComponentGeneralStyle G;
        char* Text = NULL;
        char* FontPath = "default-font.sfn";
        uintptr_t FontBuffer = 0;
        int16_t FontSize = 8;
        color_t ForegroundColor = 0;
        bool AutoWidth = true;
        bool AutoHeight = true;
        TextAlign Align = TEXTALIGNLEFT;
    } LabelStyle_t;

    struct Label_t{
        LabelStyle_t Style;
        Component* Cpnt;
        kfont_t* Font;
        int64_t TextWidth;
        int64_t TextHeight;
        int64_t TextX;
        int64_t TextY;
        uint64_t Lock;
        void UpdateText(char* Text);
        void UpdateSize(uint64_t Width, uint64_t Height);
        void UpdatePosition(point_t Position);
    };

    Label_t* Label(LabelStyle_t Style, class Component* ParentCpnt);


    /* Titlebar */
    typedef struct {
        uint32_t BackgroundColor = 0;
        uint32_t ForegroundColor = 0;
        color_t HoverColor = BackgroundColor;
        color_t ClickColor = HoverColor;
    } TitlebarStyle_t;

    struct Titlebar_t{
        uintptr_t Window;
        TitlebarStyle_t Style;
        color_t CurrentColor;

        /* Titlebar move */
        bool IsMouseDrag;
        point_t MousePosition;
        point_t WindowInitialPosition;

        Component* Cpnt;
        Box_t* MainBox;
        Flexbox_t* MainFlexbox;

        Picturebox_t* Logo;

        Label_t* Title;

        Box_t* BtnBox;
        Button_t* CloseBtn;
        Button_t* MaximizeBtn;
        Button_t* MinimizeBtn;

        void UpdateSize(uint64_t Width, uint64_t Height);
        void UpdatePosition(point_t Position);
    };

    Titlebar_t* Titlebar(uintptr_t Window, char* Title, char* Icon, TitlebarStyle_t Style, class Component* ParentCpnt);   

}

#endif