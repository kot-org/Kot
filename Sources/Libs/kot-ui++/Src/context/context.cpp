#include <kot-ui++/context.h>

namespace Ui {
    void UiContextRenderer(){
        UiContext* Context = (UiContext*)Sys_GetExternalDataThread();
        Context->IsRendering = true;
        while(Context->Renderer){
            SetGraphicEventbuffer(Context->EventBuffer, (uint64_t)Context->Cpnt, Context->Fb->Width, Context->Fb->Height, NULL, NULL);
            Context->Cpnt->Update();
            BlitGraphicEventbuffer(Context->EventBufferUse, Context->EventBuffer, NULL, NULL);
            BlitFramebuffer(Context->Fb, Context->Cpnt->GetFramebuffer(), NULL, NULL);
        }
        Context->IsRendering = false;
        Sys_Close(KSUCCESS);
    }

    void UiContextUpdate(Component* Cpnt){

    }

    void UiContextMouseEvent(class Component* Cpnt, bool IsHover, int64_t RelativePositionX, int64_t RelativePositionY, int64_t PositionX, int64_t PositionY, int64_t ZValue, uint64_t Status){
        if(IsHover){
            if(Cpnt->UiCtx->FocusCpnt != NULL){
                if(Cpnt->UiCtx->FocusCpnt != Cpnt){
                    if(Cpnt->UiCtx->FocusCpnt->MouseEvent){
                        Cpnt->UiCtx->FocusCpnt->MouseEvent(Cpnt->UiCtx->FocusCpnt, false, RelativePositionX, RelativePositionY, PositionX, PositionY, ZValue, Status);
                    }
                }
            }
            Cpnt->UiCtx->FocusCpnt = Cpnt;
        }
    }

    UiContext::UiContext(kot_framebuffer_t* fb) {
        this->Fb = fb;
        this->EventBuffer = CreateEventBuffer(fb->Width, fb->Height);
        this->EventBufferUse = CreateEventBuffer(fb->Width, fb->Height);
        this->Cpnt = new Component({.Width = (int64_t)fb->Width, .Height = (int64_t)fb->Height, .IsHidden = false}, UiContextUpdate, UiContextMouseEvent, NULL, NULL, true);
        this->Cpnt->UiCtx = this;
        this->FocusCpnt = Cpnt;
        this->Renderer = false;
        Sys_CreateThread(Sys_GetProcess(), (uintptr_t)&UiContextRenderer, PriviledgeApp, (uint64_t)this, &ThreadRenderer);
    }

    void UiContext::UpdateFramebuffer(kot_framebuffer_t* fb){
        // Warning : Don't forget to stop renderer before
        this->Fb = fb;
        this->Cpnt->UpdateFramebuffer(fb->Width, fb->Height);
        FreeEventBuffer(this->EventBuffer);
        FreeEventBuffer(this->EventBufferUse);
        this->EventBuffer = CreateEventBuffer(fb->Width, fb->Height);
        this->EventBufferUse = CreateEventBuffer(fb->Width, fb->Height);
    }

    void UiContext::UiStartRenderer(){
        if(!Renderer){
            Renderer = true;
            IsListeningEvents = true;
            Sys_ExecThread(ThreadRenderer, NULL, ExecutionTypeQueu, NULL);
        }
    }

    void UiContext::UiStopRenderer(){
        IsListeningEvents = false;
        Renderer = false;
        while(IsRendering){
            asm volatile("":::"memory");
        }
    }
}