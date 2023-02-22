#include <kot-ui++/component.h>

namespace Ui {
    void FlexboxUpdate(Component* Cpnt){
        Flexbox_t* Flexbox = (Flexbox_t*)Cpnt->ExternalData;
        if(Cpnt->IsFramebufferUpdate){
            // Draw
            Flexbox->Style.G.Width.Current = Cpnt->GetStyle()->Width.Current;
            Flexbox->Style.G.Height.Current = Cpnt->GetStyle()->Height.Current;

            Cpnt->IsFramebufferUpdate = false;
        }
        Cpnt->AbsolutePosition = {.x = Cpnt->Parent->AbsolutePosition.x + Flexbox->Style.G.Position.x, .y = Cpnt->Parent->AbsolutePosition.y + Flexbox->Style.G.Position.y};
        if(Cpnt->Childs != NULL){
            uint64_t TotalWidth = Cpnt->Style->Width.Current;
            uint64_t TotalHeight = Cpnt->Style->Height.Current;
            uint64_t TotalWidthChild = 0;
            uint64_t TotalHeightChild = 0;

            for(uint64_t i = 0; i < Cpnt->Childs->length; i++){
                Component* Child = (Component*)vector_get(Cpnt->Childs, i);
                if(Child->Style->Width.Normal < 0){
                    Child->Style->Width.Current = (Cpnt->Style->Width.Current * abs(Child->Style->Width.Normal)) / 100;
                    if(Child->Style->Width.Current < Child->Style->Width.Min){
                        Child->Style->Width.Current = Child->Style->Width.Min;
                    }else if(Child->Style->Width.Current < Child->Style->Width.Max){
                        Child->Style->Width.Current = Child->Style->Width.Max;
                    }
                }else{
                    Child->Style->Width.Current = Child->Style->Width.Normal;
                }

                if(Child->Style->Height.Normal < 0){
                    Child->Style->Height.Current = (Cpnt->Style->Height.Current * abs(Child->Style->Height.Normal)) / 100;
                    if(Child->Style->Height.Current < Child->Style->Height.Min){
                        Child->Style->Height.Current = Child->Style->Height.Min;
                    }else if(Child->Style->Height.Current < Child->Style->Height.Max){
                        Child->Style->Height.Current = Child->Style->Height.Max;
                    }
                }else{
                    Child->Style->Height.Current = Child->Style->Height.Normal;
                }

                TotalWidthChild += Child->Style->Width.Current + Child->Style->Margin.Left + Child->Style->Margin.Right;
                TotalHeightChild += Child->Style->Height.Current + Child->Style->Margin.Bottom + Child->Style->Margin.Top;
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
                            NewWidth = Child->Style->Width.Current * TotalWidth / TotalWidthChild;
                            if(NewWidth < Child->Style->Width.Min){
                                NewWidth = Child->Style->Width.Min;
                            }else if(NewWidth < Child->Style->Width.Max){
                                NewWidth = Child->Style->Width.Max;
                            }
                            XIteration += NewWidth; 
                        }else{
                            NewWidth = TotalWidth;
                        }
                        Child->UpdateFramebuffer(NewWidth, Child->Style->Height.Current);
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
                        XIteration += Child->Style->Width.Current + Child->Style->Margin.Left + Child->Style->Margin.Right;
                    }
                    break;
                }
                case Ui::Layout::CENTER:{
                    uint64_t XIteration = (TotalWidth - TotalWidthChild) / 2;
                    for(uint64_t i = 0; i < Cpnt->Childs->length; i++) {
                        Component* Child = (Component*)vector_get(Cpnt->Childs, i);
                        Child->Style->Position.x = XIteration;
                        XIteration += Child->Style->Width.Current + Child->Style->Margin.Left + Child->Style->Margin.Right;
                    }
                    break;
                }
                case Ui::Layout::RIGHT:{
                    uint64_t XIteration = TotalWidth - TotalWidthChild;
                    for(uint64_t i = 0; i < Cpnt->Childs->length; i++) {
                        Component* Child = (Component*)vector_get(Cpnt->Childs, i);
                        Child->Style->Position.x = XIteration;
                        XIteration += Child->Style->Width.Current + Child->Style->Margin.Left + Child->Style->Margin.Right;
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
                            NewHeight = Child->Style->Height.Current * TotalHeight / TotalHeightChild;
                            YIteration += NewHeight + Child->Style->Margin.Top + Child->Style->Margin.Bottom;
                            if(NewHeight < Child->Style->Height.Min){
                                NewHeight = Child->Style->Height.Min;
                            }else if(NewHeight < Child->Style->Height.Max){
                                NewHeight = Child->Style->Height.Max;
                            }
                        }else{
                            NewHeight = TotalHeight;
                        }
                        Child->UpdateFramebuffer(Child->Style->Width.Current, NewHeight);
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
                        YIteration += Child->Style->Height.Current + Child->Style->Margin.Top + Child->Style->Margin.Bottom;
                        Child->UpdateFunction(Child);
                    }
                    break;
                }
                case Ui::Layout::MIDDLE:{
                    uint64_t YIteration = (TotalHeight - TotalHeightChild) / 2;
                    for(uint64_t i = 0; i < Cpnt->Childs->length; i++) {
                        Component* Child = (Component*)vector_get(Cpnt->Childs, i);
                        Child->Style->Position.y = YIteration;
                        YIteration += Child->Style->Height.Current + Child->Style->Margin.Top + Child->Style->Margin.Bottom;
                        Child->UpdateFunction(Child);
                    }
                    break;
                }
                case Ui::Layout::BOTTOM:{
                    uint64_t YIteration = TotalHeight - TotalHeightChild;
                    for(uint64_t i = 0; i < Cpnt->Childs->length; i++) {
                        Component* Child = (Component*)vector_get(Cpnt->Childs, i);
                        Child->Style->Position.y = YIteration;
                        YIteration += Child->Style->Height.Current + Child->Style->Margin.Top + Child->Style->Margin.Bottom;
                        Child->UpdateFunction(Child);
                    }
                    break;
                }
            }
        }
        BlitFramebuffer(Cpnt->Parent->GetFramebuffer(), Cpnt->GetFramebuffer(), Cpnt->Style->Position.x, Cpnt->Style->Position.y);
    }

    Flexbox_t* Flexbox(FlexboxStyle_t Style, Component* ParentCpnt){
        Flexbox_t* Flexbox = (Flexbox_t*)malloc(sizeof(Flexbox_t));
        memcpy(&Flexbox->Style, &Style, sizeof(FlexboxStyle_t));
        Flexbox->Cpnt = new Component(Style.G, FlexboxUpdate, NULL, (uintptr_t)Flexbox, ParentCpnt, false);
        return Flexbox;
    }

}