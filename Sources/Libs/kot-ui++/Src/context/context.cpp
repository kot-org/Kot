#include <kot-ui++/context.h>

namespace Ui {
    void UiContextRenderer(){
        UiContext* Context = (UiContext*)Sys_GetExternalDataThread();
        while(Context->IsRendering){
            Context->Cpnt->Update();
            BlitFramebuffer(Context->Fb, Context->Cpnt->GetFramebuffer(), NULL, NULL);
        }
        Sys_Close(KSUCCESS);
    }

    void UiContextUpdate(Component* Cpnt){

    }

    UiContext::UiContext(framebuffer_t* fb) {
        this->Fb = fb;
        this->EventBuffer = CreateEventBuffer(fb->Width, fb->Height);
        this->Cpnt = new Component({.Width = fb->Width, .Height = fb->Height, .IsVisible = true}, UiContextUpdate, NULL, NULL, this, true);
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