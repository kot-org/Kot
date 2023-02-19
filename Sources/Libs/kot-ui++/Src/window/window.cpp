#include <kot-ui++/window.h>

#include <kot-graphics/orb.h>

#include <kot/uisd/srvs/system.h>

#include <kot-ui++/component.h>

namespace UiWindow {

    void EventHandler(enum Window_Event EventType, uint64_t GP0, uint64_t GP1, uint64_t GP2, uint64_t GP3, uint64_t GP4){
        Window* Wid = (Window*)Sys_GetExternalDataThread();
        Wid->Handler(EventType, GP0, GP1, GP2, GP3, GP4);
        Sys_Event_Close();
    }

    void ButtonTest(struct Ui::Button_t* Button, Ui::ButtonEventType EventType){
        if(EventType == Ui::ButtonEventTypeLeftClick){
            Printlog("Click");
        }
    }

    Window::Window(char* title, uint32_t Width, uint32_t Height, uint32_t XPosition, uint32_t YPosition){
        // Setup event
        Sys_Event_Create(&WindowEvent);
        Sys_CreateThread(Sys_GetProcess(), (uintptr_t)&EventHandler, PriviledgeApp, (uint64_t)this, &WindowHandlerThread);
        Sys_Event_Bind(WindowEvent, WindowHandlerThread, false);
        IsListeningEvents = false;

        // Setup window
        this->Wid = CreateWindow(WindowEvent, Window_Type_Default);
        WindowChangePosition(this->Wid, XPosition, YPosition);

        IsBorders = true;
        if(IsBorders){
            ResizeWindow(this->Wid, Width + 2, Height + 2);
            BordersCtx = CreateGraphicContext(&Wid->Framebuffer);
            framebuffer_t* FramebufferWithoutBorder = (framebuffer_t*)malloc(sizeof(framebuffer_t));
            FramebufferWithoutBorder->Bpp = Wid->Framebuffer.Bpp;
            FramebufferWithoutBorder->Btpp = Wid->Framebuffer.Btpp;
            FramebufferWithoutBorder->Width = Wid->Framebuffer.Width - 2;
            FramebufferWithoutBorder->Height = Wid->Framebuffer.Height - 2;
            FramebufferWithoutBorder->Pitch = Wid->Framebuffer.Pitch;
            FramebufferWithoutBorder->Buffer = (uintptr_t)((uint64_t)Wid->Framebuffer.Buffer + Wid->Framebuffer.Btpp + Wid->Framebuffer.Pitch);
            DrawBorders(WIN_BDCOLOR_ONBLUR);

            GraphicCtx = CreateGraphicContext(FramebufferWithoutBorder);
            UiCtx = new Ui::UiContext(FramebufferWithoutBorder);
        }else{
            ResizeWindow(this->Wid, Width, Height);
            GraphicCtx = CreateGraphicContext(&Wid->Framebuffer);
            UiCtx = new Ui::UiContext(&Wid->Framebuffer);
        }

        // Test

        /* auto imgtest = Ui::Picturebox("kotlogo.tga", Ui::PictureboxType::_TGA, { .Width = 256, .Height = 256 });
        this->SetContent(imgtest); */
        
        //Titlebar = Ui::Titlebar(title, { .BackgroundColor = 0xffffff, .ForegroundColor = WIN_TBCOLOR_ONBLUR }, UiCtx);
        //auto Flex = Ui::Flexbox({.Width = Width, .Height = Height, .Align = {.x = Ui::Layout::FILLHORIZONTAL, .y = Ui::Layout::FILLVERTICAL}}, UiCtx);
        //this->SetContent(Flex->Cpnt);
        auto Grid = Ui::Gridbox({.Width = Width, .Height = Height, .CaseWidth = 50, .CaseHeight = 50 }, UiCtx);
        this->SetContent(Grid->Cpnt);
        for(uint64_t y = 0; y < 5; y++){
            for(uint64_t x = 0; x < 5; x++){
                auto iconBox = Ui::Button({ .Width = 20, .Height = 20, .ClickColor = (color_t)0x00ff00, .HoverColor = (color_t)0xff0000, .BackgroundColor = (color_t)0xffffff, .Position{.x = (int64_t)x,  .y = (int64_t)y}, .IsVisible = true, .OnClick = ButtonTest }, UiCtx);
                //auto imgtest = Ui::Picturebox("kotlogo.tga", Ui::PictureboxType::_TGA, { .Width = 20, .Height = 20, .Position{.x = (int64_t)x,  .y = (int64_t)y}, .IsVisible = true, .Fit=Ui::PICTUREFILL }, UiCtx);
                Grid->Cpnt->AddChild(iconBox->Cpnt);
                //this->SetContent(iconBox->Cpnt);
            }
        }
        // auto imageBox = Ui::Picturebox("kotlogo.tga", Ui::PictureboxType::_TGA, { .Width = 205, .Height = 205, .Fit = Ui::PictureboxFit::FILL, .IsVisible = true }, UiCtx);
        // this->SetContent(imageBox->Cpnt);
        //this->SetContent(Titlebar->Cpnt);
 
/*         auto wrapper = Ui::Box({ .Width = this->UiCtx->fb->Width, .Height = this->UiCtx->fb->Height - titlebar->GetStyle()->Height, .color = WIN_BGCOLOR_ONFOCUS });

        auto flexbox = UiLayout::Flexbox({}, Ui::Layout::HORIZONTAL);

        auto Box = Ui::Box({ .Width = 20, .Height = 20, .color = 0xFFFF00 });
        flexbox->AddChild(box);

        wrapper->AddChild(flexbox);

        this->SetContent(wrapper); */

        UiCtx->UiStartRenderer();
        ChangeVisibilityWindow(this->Wid, true);
        IsListeningEvents = true;
    }

    void Window::DrawBorders(uint32_t Color){
        if(IsBorders){
            ctxDrawRect(BordersCtx, 0, 0, BordersCtx->Width - 1, BordersCtx->Height - 1, Color);
        }
    }

    void Window::SetContent(Ui::Component* content) {
        Ui::Component* windowCpnt = this->UiCtx->Cpnt;

        windowCpnt->AddChild(content);
    }

    void Window::Handler(enum Window_Event EventType, uint64_t GP0, uint64_t GP1, uint64_t GP2, uint64_t GP3, uint64_t GP4){
        if(IsListeningEvents){
            switch (EventType){
                case Window_Event_Focus:
                    HandlerFocus(GP0);
                    break;
                case Window_Event_Mouse:
                    HandlerMouse(GP0, GP1, GP2, GP3);
                    break;
                case Window_Event_Keyboard:
                    // TODO
                    break;
                default:
                    break;
            }
        }
    }

    void Window::HandlerFocus(bool IsFocus){
        if(IsFocus){
            DrawBorders(WIN_BDCOLOR_ONFOCUS);
            //Titlebar->GetStyle()->BackgroundColor = WIN_TBCOLOR_ONFOCUS;
        }else{
            DrawBorders(WIN_BDCOLOR_ONBLUR);
            //Titlebar->GetStyle()->BackgroundColor = WIN_TBCOLOR_ONBLUR;
        }

        //Titlebar->Update();
    }

    void Window::HandlerMouse(uint64_t PositionX, uint64_t PositionY, uint64_t ZValue, uint64_t Status){
        int64_t RelativePostionX = PositionX - Wid->Position.x;
        int64_t RelativePostionY = PositionY - Wid->Position.y; 

        if(RelativePostionX >= 0 && RelativePostionY >= 0 && RelativePostionX < UiCtx->EventBuffer->Width && RelativePostionY < UiCtx->EventBuffer->Height){
            Ui::Component* Component = (Ui::Component*)GetEventData(UiCtx->EventBufferUse, RelativePostionX, RelativePostionY);
            if(Component){
                if(Component->MouseEvent){
                    Component->MouseEvent(Component, true, RelativePostionX, RelativePostionY, PositionX, PositionY, ZValue, Status);
                }
            }
        }
    }

}