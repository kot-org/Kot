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

    Window::Window(char* Title, char* Icon, uint32_t Width, uint32_t Height, uint32_t XPosition, uint32_t YPosition){
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

        if(Icon == NULL){
            Icon = "kotlogo.tga";
        }

        Titlebar = Ui::Titlebar(Title, Icon, {.BackgroundColor = WIN_TBCOLOR_ONBLUR, .ForegroundColor = 0xffffffff}, UiCtx->Cpnt);

        // Content
        Cpnt = Ui::Box(
            {
                .G = {
                        .Width = -100,
                        .Maxwidth = NO_MAXIMUM,
                        .Height = Height - 25,
                        .AutoPosition = true,
                        .IsHidden = false,
                    },
                .HoverColor = 0xffffff,
            }
        , UiCtx->Cpnt)->Cpnt;

        UiCtx->UiStartRenderer();
        ChangeVisibilityWindow(this->Wid, true);
        IsListeningEvents = true;
    }

    void Window::DrawBorders(uint32_t Color){
        if(IsBorders){
            ctxDrawRect(BordersCtx, 0, 0, BordersCtx->Width - 1, BordersCtx->Height - 1, Color);
        }
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
        int64_t RelativePositionX = PositionX - Wid->Position.x;
        int64_t RelativePositionY = PositionY - Wid->Position.y; 

        Ui::Component* Component = (Ui::Component*)GetEventData(UiCtx->EventBufferUse, RelativePositionX, RelativePositionY);
        if(RelativePositionX >= 0 && RelativePositionY >= 0 && RelativePositionX < UiCtx->EventBuffer->Width && RelativePositionY < UiCtx->EventBuffer->Height){
            if(Component){
                if(Component->MouseEvent){
                    Component->MouseEvent(Component, true, RelativePositionX, RelativePositionY, PositionX, PositionY, ZValue, Status);
                }
            }
        }else if(UiCtx->FocusCpnt->MouseEvent){
            Component->UiCtx->FocusCpnt->MouseEvent(Component->UiCtx->FocusCpnt, false, RelativePositionX, RelativePositionY, PositionX, PositionY, ZValue, Status);
        }
    }

}