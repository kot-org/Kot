#include <kot-ui++/component.h>

namespace Ui {
    // Todo calculate width and height in global update
    void GridboxUpdate(Component* Cpnt){
        Gridbox_t* Gridbox = (Gridbox_t*)Cpnt->ExternalData;
        if(Cpnt->IsFramebufferUpdate){
            // Draw
            Gridbox->Style.G.Width.Current = Cpnt->GetStyle()->Width.Current;
            Gridbox->Style.G.Height.Current = Cpnt->GetStyle()->Height.Current;

            Cpnt->IsFramebufferUpdate = false;
        }
        Cpnt->AbsolutePosition = {.x = Cpnt->Parent->AbsolutePosition.x + Cpnt->Style->Position.x, .y = Cpnt->Parent->AbsolutePosition.y + Cpnt->Style->Position.y};
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
        BlitFramebuffer(Cpnt->Parent->GetFramebuffer(), Cpnt->GetFramebuffer(), Cpnt->Style->Position.x, Cpnt->Style->Position.y);
    }

    Gridbox_t* Gridbox(GridboxStyle_t Style, Component* ParentCpnt){
        Gridbox_t* Gridbox = (Gridbox_t*)malloc(sizeof(Gridbox_t));
        memcpy(&Gridbox->Style, &Style, sizeof(GridboxStyle_t));
        Gridbox->Cpnt = new Component(Style.G, GridboxUpdate, NULL, (uintptr_t)Gridbox, ParentCpnt, false);
        return Gridbox;
    }

}