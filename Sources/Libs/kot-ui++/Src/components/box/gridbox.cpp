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

        Cpnt->AbsolutePosition = {.x = (int64_t)(Cpnt->Parent->AbsolutePosition.x + Cpnt->Style->Position.x + Cpnt->Style->Margin.Left - Cpnt->Style->Margin.Right), .y = (int64_t)(Cpnt->Parent->AbsolutePosition.y + Cpnt->Style->Position.y + Cpnt->Style->Margin.Top - Cpnt->Style->Margin.Bottom)};
        Cpnt->FramebufferRelativePosition = {.x = Cpnt->Parent->FramebufferRelativePosition.x + Cpnt->Style->Position.x, .y = Cpnt->Parent->FramebufferRelativePosition.y + Cpnt->Style->Position.y};

        if(Cpnt->Childs != NULL){
            uint64_t CaseWidth = Gridbox->Style.CaseWidth;
            uint64_t CaseHeight = Gridbox->Style.CaseHeight;
            if(Gridbox->Style.CaseWidth == GRIDBOX_CASE_SIZE_AUTO || Gridbox->Style.CaseHeight == GRIDBOX_CASE_SIZE_AUTO){
                uint64_t MaxPostionX = 0;
                uint64_t MaxPostionY = 0;
                for(uint64_t i = 0; i < Cpnt->Childs->length; i++){
                    Component* Child = (Component*)vector_get(Cpnt->Childs, i);
                    if(Child->Style->Position.x > MaxPostionX){
                        MaxPostionX = Child->Style->Position.x;
                    }
                    if(Child->Style->Position.y > MaxPostionY){
                        MaxPostionY = Child->Style->Position.y;
                    }
                }
                if(Gridbox->Style.CaseWidth == GRIDBOX_CASE_SIZE_AUTO){
                    CaseWidth = (Cpnt->Style->Currentwidth / (MaxPostionX + 1)) - Gridbox->Style.SpaceBetweenCaseHorizontal;
                }
                
                if(Gridbox->Style.CaseHeight == GRIDBOX_CASE_SIZE_AUTO){
                    CaseHeight = (Cpnt->Style->Currentheight / (MaxPostionY + 1)) - Gridbox->Style.SpaceBetweenCaseVertical;
                }
            }
            for(uint64_t i = 0; i < Cpnt->Childs->length; i++){
                Component* Child = (Component*)vector_get(Cpnt->Childs, i);
                point_t GridPosition;
                GridPosition.x = Child->Style->Position.x;
                GridPosition.y = Child->Style->Position.y;
                Child->Style->Position.x *= CaseWidth;
                Child->Style->Position.y *= CaseHeight;
                Child->UpdateFunction(Child);
                Child->Update();
                Child->Style->Position.x = GridPosition.x;
                Child->Style->Position.y = GridPosition.y;
            }
        }
        Cpnt->Parent->IsDrawUpdate = Cpnt->IsDrawUpdate;
    }

    Gridbox_t* Gridbox(GridboxStyle_t Style, Component* ParentCpnt){
        Gridbox_t* Gridbox = (Gridbox_t*)malloc(sizeof(Gridbox_t));
        memcpy(&Gridbox->Style, &Style, sizeof(GridboxStyle_t));
        Gridbox->Cpnt = new Component(Style.G, GridboxUpdate, NULL, (uintptr_t)Gridbox, ParentCpnt, false);
        return Gridbox;
    }

}