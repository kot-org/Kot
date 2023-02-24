#include <kot-ui++/component.h>

namespace Ui {
    void FlexboxUpdate(Component* Cpnt){
        Flexbox_t* Flexbox = (Flexbox_t*)Cpnt->ExternalData;
        if(Cpnt->IsFramebufferUpdate){
            // Draw
            Flexbox->Style.G.Currentwidth = Cpnt->GetStyle()->Currentwidth;
            Flexbox->Style.G.Currentheight = Cpnt->GetStyle()->Currentheight;

            Cpnt->IsFramebufferUpdate = false;
        }
        Cpnt->AbsolutePosition = {.x = Cpnt->Parent->AbsolutePosition.x + Flexbox->Style.G.Position.x, .y = Cpnt->Parent->AbsolutePosition.y + Flexbox->Style.G.Position.y};
        if(Cpnt->Childs != NULL){
            uint64_t TotalWidth = Cpnt->Style->Currentwidth;
            uint64_t TotalHeight = Cpnt->Style->Currentheight;
            uint64_t TotalWidthChild = 0;
            uint64_t TotalHeightChild = 0;

            for(uint64_t i = 0; i < Cpnt->Childs->length; i++){
                Component* Child = (Component*)vector_get(Cpnt->Childs, i);
                if(Child->Style->Width < 0){
                    Child->Style->Currentwidth = (Cpnt->Style->Currentwidth * abs(Child->Style->Width)) / 100;
                    if(Child->Style->Currentwidth < Child->Style->Minwidth){
                        Child->Style->Currentwidth = Child->Style->Minwidth;
                    }else if(Child->Style->Currentwidth > Child->Style->Maxwidth){
                        Child->Style->Currentwidth = Child->Style->Maxwidth;
                    }
                }else{
                    Child->Style->Currentwidth = Child->Style->Width;
                }

                if(Child->Style->Height < 0){
                    Child->Style->Currentheight = (Cpnt->Style->Currentheight * abs(Child->Style->Height)) / 100;
                    if(Child->Style->Currentheight < Child->Style->Minheight){
                        Child->Style->Currentheight = Child->Style->Minheight;
                    }else if(Child->Style->Currentheight < Child->Style->Maxheight){
                        Child->Style->Currentheight = Child->Style->Maxheight;
                    }
                }else{
                    Child->Style->Currentheight = Child->Style->Height;
                }

                TotalWidthChild += Child->Style->Currentwidth + Child->Style->Margin.Left + Child->Style->Margin.Right;
                TotalHeightChild += Child->Style->Currentheight + Child->Style->Margin.Bottom + Child->Style->Margin.Top;
            }
            bool IterateX = Flexbox->Style.Align.y == Ui::Layout::FILLVERTICAL ||  Flexbox->Style.Align.y == Ui::Layout::BETWEENVERTICAL || Flexbox->Style.Align.y == Ui::Layout::AROUNDVERTICAL;
            bool IterateY = !IterateX;
            // Do not update in x only in y
            switch(Flexbox->Style.Align.x){
                case Ui::Layout::FILLHORIZONTAL:{
                    uint64_t XIteration = 0;
                    for(uint64_t i = 0; i < Cpnt->Childs->length; i++) {
                        Component* Child = (Component*)vector_get(Cpnt->Childs, i);
                        uint64_t NewWidth;
                        Child->Style->Position.x = XIteration;
                        if(IterateX){
                            NewWidth = Child->Style->Currentwidth * TotalWidth / TotalWidthChild;
                            if(NewWidth < Child->Style->Minwidth){
                                NewWidth = Child->Style->Minwidth;
                            }else if(NewWidth < Child->Style->Maxwidth){
                                NewWidth = Child->Style->Maxwidth;
                            }
                            XIteration += NewWidth; 
                        }else{
                            NewWidth = TotalWidth;
                        }
                        Child->UpdateFramebuffer(NewWidth, Child->Style->Currentheight);
                    }
                    break;
                }
                case Ui::Layout::BETWEENHORIZONTAL:{
                    uint64_t SpaceFreePerChild = (TotalWidth - TotalWidthChild) / (Cpnt->Childs->length - 1);
                    uint64_t XIteration = 0;
                    uint64_t i = 0;
                    for(; i < Cpnt->Childs->length / 2; i++) {
                        Component* Child = (Component*)vector_get(Cpnt->Childs, i);
                        Child->Style->Position.x = XIteration;
                        if(IterateX) XIteration += SpaceFreePerChild;
                    }

                    if(IterateX) XIteration += (TotalWidth - TotalWidthChild) % (Cpnt->Childs->length - 1);

                    for(; i < Cpnt->Childs->length; i++) {
                        Component* Child = (Component*)vector_get(Cpnt->Childs, i);
                        Child->Style->Position.x = XIteration;
                        if(IterateX) XIteration += SpaceFreePerChild;
                    }
                    break;
                }
                case Ui::Layout::AROUNDHORIZONTAL:{
                    uint64_t SpaceFreePerChild = (TotalWidth - TotalWidthChild) / Cpnt->Childs->length;
                    uint64_t XIteration = SpaceFreePerChild / 2;
                    uint64_t i = 0;
                    for(; i < Cpnt->Childs->length / 2; i++) {
                        Component* Child = (Component*)vector_get(Cpnt->Childs, i);
                        Child->Style->Position.x = XIteration;
                        if(IterateX) XIteration += SpaceFreePerChild;
                    }

                    if(IterateX) XIteration += (TotalWidth - TotalWidthChild) % Cpnt->Childs->length;

                    for(; i < Cpnt->Childs->length; i++) {
                        Component* Child = (Component*)vector_get(Cpnt->Childs, i);
                        Child->Style->Position.x = XIteration;
                        if(IterateX) XIteration += SpaceFreePerChild;
                    }
                    break;
                }
                case Ui::Layout::LEFT:{
                    uint64_t XIteration = 0;
                    for(uint64_t i = 0; i < Cpnt->Childs->length; i++) {
                        Component* Child = (Component*)vector_get(Cpnt->Childs, i);
                        Child->Style->Position.x = XIteration;
                        XIteration += Child->Style->Currentwidth + Child->Style->Margin.Left + Child->Style->Margin.Right;
                    }
                    break;
                }
                case Ui::Layout::CENTER:{
                    uint64_t XIteration = (TotalWidth - TotalWidthChild) / 2;
                    for(uint64_t i = 0; i < Cpnt->Childs->length; i++) {
                        Component* Child = (Component*)vector_get(Cpnt->Childs, i);
                        Child->Style->Position.x = XIteration;
                        XIteration += Child->Style->Currentwidth + Child->Style->Margin.Left + Child->Style->Margin.Right;
                    }
                    break;
                }
                case Ui::Layout::RIGHT:{
                    uint64_t XIteration = TotalWidth - TotalWidthChild;
                    for(uint64_t i = 0; i < Cpnt->Childs->length; i++) {
                        Component* Child = (Component*)vector_get(Cpnt->Childs, i);
                        Child->Style->Position.x = XIteration;
                        XIteration += Child->Style->Currentwidth + Child->Style->Margin.Left + Child->Style->Margin.Right;
                    }
                    break;
                }
            }
            switch(Flexbox->Style.Align.y){
                case Ui::Layout::FILLVERTICAL:{
                    uint64_t YIteration = 0;
                    for(uint64_t i = 0; i < Cpnt->Childs->length; i++) {
                        Component* Child = (Component*)vector_get(Cpnt->Childs, i);
                        uint64_t NewHeight;
                        Child->Style->Position.y = YIteration;
                        if(IterateY){
                            NewHeight = Child->Style->Currentheight * TotalHeight / TotalHeightChild;
                            YIteration += NewHeight + Child->Style->Margin.Top + Child->Style->Margin.Bottom;
                            if(NewHeight < Child->Style->Minheight){
                                NewHeight = Child->Style->Minheight;
                            }else if(NewHeight < Child->Style->Maxheight){
                                NewHeight = Child->Style->Maxheight;
                            }
                        }else{
                            NewHeight = TotalHeight;
                        }
                        Child->UpdateFramebuffer(Child->Style->Currentwidth, NewHeight);
                        Child->UpdateFunction(Child);
                        Child->Update();
                    }
                    break;
                }
                case Ui::Layout::BETWEENVERTICAL:{
                    uint64_t SpaceFreePerChild = (TotalHeight - TotalHeightChild) / (Cpnt->Childs->length - 1);
                    uint64_t YIteration = 0;
                    uint64_t i = 0;
                    for(; i < Cpnt->Childs->length / 2; i++) {
                        Component* Child = (Component*)vector_get(Cpnt->Childs, i);
                        Child->Style->Position.y = YIteration;
                        if(IterateY) YIteration += SpaceFreePerChild;
                        Child->UpdateFunction(Child);
                    }

                    if(IterateY) YIteration += (TotalHeight - TotalHeightChild) % (Cpnt->Childs->length - 1);

                    for(; i < Cpnt->Childs->length; i++) {
                        Component* Child = (Component*)vector_get(Cpnt->Childs, i);
                        Child->Style->Position.y = YIteration;
                        if(IterateY) YIteration += SpaceFreePerChild;
                        Child->UpdateFunction(Child);
                    }
                    break;
                }
                case Ui::Layout::AROUNDVERTICAL:{
                    uint64_t SpaceFreePerChild = (TotalHeight - TotalHeightChild) / Cpnt->Childs->length;
                    uint64_t YIteration = SpaceFreePerChild / 2;
                    uint64_t i = 0;
                    for(; i < Cpnt->Childs->length / 2; i++) {
                        Component* Child = (Component*)vector_get(Cpnt->Childs, i);
                        Child->Style->Position.y = YIteration;
                        if(IterateY) YIteration += SpaceFreePerChild;
                        Child->UpdateFunction(Child);
                    }

                    if(IterateY) YIteration += (TotalHeight - TotalHeightChild) % Cpnt->Childs->length;

                    for(; i < Cpnt->Childs->length; i++) {
                        Component* Child = (Component*)vector_get(Cpnt->Childs, i);
                        Child->Style->Position.y = YIteration;
                        if(IterateY) YIteration += SpaceFreePerChild;
                        Child->UpdateFunction(Child);
                    }
                    break;
                }
                case Ui::Layout::TOP:{
                    uint64_t YIteration = 0;
                    for(uint64_t i = 0; i < Cpnt->Childs->length; i++) {
                        Component* Child = (Component*)vector_get(Cpnt->Childs, i);
                        Child->Style->Position.y = YIteration;
                        YIteration += Child->Style->Currentheight + Child->Style->Margin.Top + Child->Style->Margin.Bottom;
                        Child->UpdateFunction(Child);
                    }
                    break;
                }
                case Ui::Layout::MIDDLE:{
                    uint64_t YIteration = (TotalHeight - TotalHeightChild) / 2;
                    for(uint64_t i = 0; i < Cpnt->Childs->length; i++) {
                        Component* Child = (Component*)vector_get(Cpnt->Childs, i);
                        Child->Style->Position.y = YIteration;
                        YIteration += Child->Style->Currentheight + Child->Style->Margin.Top + Child->Style->Margin.Bottom;
                        Child->UpdateFunction(Child);
                    }
                    break;
                }
                case Ui::Layout::BOTTOM:{
                    uint64_t YIteration = TotalHeight - TotalHeightChild;
                    for(uint64_t i = 0; i < Cpnt->Childs->length; i++) {
                        Component* Child = (Component*)vector_get(Cpnt->Childs, i);
                        Child->Style->Position.y = YIteration;
                        YIteration += Child->Style->Currentheight + Child->Style->Margin.Top + Child->Style->Margin.Bottom;
                        Child->UpdateFunction(Child);
                    }
                    break;
                }
            }
        }
        BlitFramebuffer(Cpnt->Parent->GetFramebuffer(), Cpnt->GetFramebuffer(), (int64_t)(Cpnt->Style->Position.x + Cpnt->Style->Margin.Left), (int64_t)(Cpnt->Style->Position.y + Cpnt->Style->Margin.Top));
    }

    Flexbox_t* Flexbox(FlexboxStyle_t Style, Component* ParentCpnt){
        Flexbox_t* Flexbox = (Flexbox_t*)malloc(sizeof(Flexbox_t));
        memcpy(&Flexbox->Style, &Style, sizeof(FlexboxStyle_t));
        Flexbox->Cpnt = new Component(Style.G, FlexboxUpdate, NULL, (uintptr_t)Flexbox, ParentCpnt, false, true);
        return Flexbox;
    }

}