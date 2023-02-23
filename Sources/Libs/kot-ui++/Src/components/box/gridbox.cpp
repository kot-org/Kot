#include <kot-ui++/component.h>

namespace Ui {
    // Todo calculate width and height in global update
    void GridboxUpdate(Component* Cpnt){
        Gridbox_t* Gridbox = (Gridbox_t*)Cpnt->ExternalData;
        if(Cpnt->IsFramebufferUpdate){
            // Draw
            Gridbox->Style.G.Currentwidth = Cpnt->GetStyle()->Currentwidth;
            Gridbox->Style.G.Currentheight = Cpnt->GetStyle()->Currentheight;

            Cpnt->IsFramebufferUpdate = false;
        }
        Cpnt->AbsolutePosition = {.x = (int64_t)(Cpnt->Parent->AbsolutePosition.x + Cpnt->Style->Position.x + Cpnt->Style->Margin.Left), .y = (int64_t)(Cpnt->Parent->AbsolutePosition.y + Cpnt->Style->Position.y + Cpnt->Style->Margin.Top)};
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
        BlitFramebuffer(Cpnt->Parent->GetFramebuffer(), Cpnt->GetFramebuffer(), (int64_t)(Cpnt->Style->Position.x + Cpnt->Style->Margin.Left), (int64_t)(Cpnt->Style->Position.y + Cpnt->Style->Margin.Top));
    }

    Gridbox_t* Gridbox(GridboxStyle_t Style, Component* ParentCpnt){
        Gridbox_t* Gridbox = (Gridbox_t*)malloc(sizeof(Gridbox_t));
        memcpy(&Gridbox->Style, &Style, sizeof(GridboxStyle_t));
        Gridbox->Cpnt = new Component(Style.G, GridboxUpdate, NULL, (uintptr_t)Gridbox, ParentCpnt, false, true);
        return Gridbox;
    }

}