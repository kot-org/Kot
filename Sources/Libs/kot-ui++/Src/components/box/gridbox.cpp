#include <kot-ui++/component.h>
#include <kot/stdio.h>

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
            int64_t CaseWidth = Gridbox->Style.CaseWidth;
            int64_t CaseHeight = Gridbox->Style.CaseHeight;
            if(CaseWidth < 0){
                CaseWidth = (Cpnt->Style->Currentwidth / abs(CaseWidth)) - Gridbox->Style.SpaceBetweenCaseHorizontal - Gridbox->Style.SpaceBetweenCaseHorizontal / abs(CaseWidth);
            }
            if(CaseHeight < 0){
                CaseHeight = (Cpnt->Style->Currentheight / abs(CaseHeight)) - Gridbox->Style.SpaceBetweenCaseVertical - Gridbox->Style.SpaceBetweenCaseVertical / abs(CaseHeight);
            }
            for(uint64_t i = 0; i < Cpnt->Childs->length; i++){
                Component* Child = (Component*)vector_get(Cpnt->Childs, i);
                point_t GridPosition;
                GridPosition.x = Child->Style->Position.x;
                GridPosition.y = Child->Style->Position.y;
                Child->Style->Position.x = Child->Style->Position.x * CaseWidth + (Child->Style->Position.x + 1) * Gridbox->Style.SpaceBetweenCaseHorizontal;
                Child->Style->Position.y = Child->Style->Position.y * CaseHeight + (Child->Style->Position.y + 1) * Gridbox->Style.SpaceBetweenCaseVertical;

                if(Child->Style->Width < 0){
                    Child->Style->Currentwidth = (CaseWidth * abs(Child->Style->Width)) / 100;
                    if(Child->Style->Currentwidth < Child->Style->Minwidth){
                        Child->Style->Currentwidth = Child->Style->Minwidth;
                    }else if(Child->Style->Currentwidth > Child->Style->Maxwidth){
                        Child->Style->Currentwidth = Child->Style->Maxwidth;
                    }
                }else{
                    Child->Style->Currentwidth = Child->Style->Width;
                }

                if(Child->Style->Height < 0){
                    Child->Style->Currentheight = (CaseHeight * abs(Child->Style->Height)) / 100;
                    if(Child->Style->Currentheight < Child->Style->Minheight){
                        Child->Style->Currentheight = Child->Style->Minheight;
                    }else if(Child->Style->Currentheight > Child->Style->Maxheight){
                        Child->Style->Currentheight = Child->Style->Maxheight;
                    }
                }else{
                    Child->Style->Currentheight = Child->Style->Height;
                }
                Child->UpdateFramebuffer(Child->Style->Currentwidth, Child->Style->Currentheight);
                Child->UpdateFunction(Child);
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