#include <kot-ui++/context.h>

namespace Ui {
    void UiContextRenderer(){
        UiContext* Context = (UiContext*)Sys_GetExternalDataThread();
        while(Context->IsRendering){
            SetGraphicEventbuffer(Context->EventBuffer, (uint64_t)Context->Cpnt, Context->Fb->Width, Context->Fb->Height, NULL, NULL);
            Context->Cpnt->Update();
            BlitGraphicEventbuffer(Context->EventBufferUse, Context->EventBuffer, NULL, NULL);
            BlitFramebuffer(Context->Fb, Context->Cpnt->GetFramebuffer(), NULL, NULL);
        }
        Sys_Close(KSUCCESS);
    }

    void UiContextUpdate(Component* Cpnt){

    }

    void UiContextMouseEvent(class Component* Cpnt, bool IsHover, int64_t RelativePositionX, int64_t RelativePositionY, int64_t PositionX, int64_t PositionY, int64_t ZValue, uint64_t Status){
        if(IsHover){
            if(Cpnt->UiCtx->FocusCpnt != Cpnt){
                if(Cpnt->UiCtx->FocusCpnt->MouseEvent){
                    Cpnt->UiCtx->FocusCpnt->MouseEvent(Cpnt->UiCtx->FocusCpnt, false, RelativePositionX, RelativePositionY, PositionX, PositionY, ZValue, Status);
                }
            }
            Cpnt->UiCtx->FocusCpnt = Cpnt;
        }
    }

    UiContext::UiContext(framebuffer_t* fb) {
        this->Fb = fb;
        this->EventBuffer = CreateEventBuffer(fb->Width, fb->Height);
        this->EventBufferUse = CreateEventBuffer(fb->Width, fb->Height);
        this->Cpnt = new Component({.Width = (int64_t)fb->Width, .Height = (int64_t)fb->Height, .IsHidden = false}, UiContextUpdate, UiContextMouseEvent, NULL, NULL, true, true);
        this->Cpnt->UiCtx = this;
        this->FocusCpnt = Cpnt;
        Sys_CreateThread(Sys_GetProcess(), (uintptr_t)&UiContextRenderer, PriviledgeApp, (uint64_t)this, &ThreadRenderer);
    }

    void UiContext::UiStartRenderer(){
        if(!IsRendering){
            IsRendering = true;
            Sys_ExecThread(ThreadRenderer, NULL, ExecutionTypeQueu, NULL);
        }
    }

    void UiContext::UiStopRenderer(){
        IsRendering = false;
    }
}