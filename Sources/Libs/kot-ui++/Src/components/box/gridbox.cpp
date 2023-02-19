#include <kot-ui++/component.h>

namespace Ui {

    void GridboxUpdate(Component* Cpnt){
        Gridbox_t* Gridbox = (Gridbox_t*)Cpnt->ExternalData;
        if(Cpnt->IsFramebufferUpdate){
            // Draw
            Gridbox->Style.Width = Cpnt->GetStyle()->Width;
            Gridbox->Style.Height = Cpnt->GetStyle()->Height;

            Cpnt->IsFramebufferUpdate = false;
        }
        Cpnt->AbsolutePosition = {.x = Cpnt->Parent->AbsolutePosition.x + Cpnt->Style->Position.x, .y = Cpnt->Parent->AbsolutePosition.y + Cpnt->Style->Position.y};
        BlitFramebuffer(Cpnt->Parent->GetFramebuffer(), Cpnt->GetFramebuffer(), Cpnt->Style->Position.x, Cpnt->Style->Position.y);
        if(Cpnt->Childs != NULL){
            for(uint64_t i = 0; i < Cpnt->Childs->length; i++){
                Component* Child = (Component*)vector_get(Cpnt->Childs, i);
                point_t GridPosition;
                GridPosition.x = Child->Style->Position.x;
                GridPosition.y = Child->Style->Position.y;
                Child->Style->Position.x *= Gridbox->Style.CaseWidth;
                Child->Style->Position.y *= Gridbox->Style.CaseHeight;
                Child->UpdateFunction(Child);
                Child->Update();
                Child->Style->Position.x = GridPosition.x;
                Child->Style->Position.y = GridPosition.y;
            }
        }
    }

    Gridbox_t* Gridbox(GridboxStyle_t Style, UiContext* ParentUiContex){
        Gridbox_t* Gridbox = (Gridbox_t*)malloc(sizeof(Gridbox_t));
        memcpy(&Gridbox->Style, &Style, sizeof(GridboxStyle_t));
        Gridbox->Cpnt = new Component({ .Width = Style.Width, .Height = Style.Height, .IsVisible = Style.IsVisible, .Position = {.x = Style.Position.x, .y = Style.Position.y}}, GridboxUpdate, NULL, (uintptr_t)Gridbox, ParentUiContex, false);
        return Gridbox;
    }

}