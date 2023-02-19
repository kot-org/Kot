#include <kot-ui++/component.h>

namespace Ui {
    void FlexboxUpdate(Component* Cpnt){
        Flexbox_t* Flexbox = (Flexbox_t*)Cpnt->ExternalData;
        if(Cpnt->IsFramebufferUpdate){
            // Draw
            Flexbox->Style.Width = Cpnt->GetStyle()->Width;
            Flexbox->Style.Height = Cpnt->GetStyle()->Height;

            Cpnt->IsFramebufferUpdate = false;
        }
        Cpnt->AbsolutePosition = {.x = Cpnt->Parent->AbsolutePosition.x + Flexbox->Style.Position.x, .y = Cpnt->Parent->AbsolutePosition.y + Flexbox->Style.Position.y};
        if(Cpnt->Childs != NULL){
            uint64_t TotalWidth = Cpnt->Style->Width;
            uint64_t TotalHeight = Cpnt->Style->Height;
            uint64_t TotalWidthChild = 0;
            uint64_t TotalHeightChild = 0;

            for(uint64_t i = 0; i < Cpnt->Childs->length; i++){
                Component* Child = (Component*)vector_get(Cpnt->Childs, i);
                TotalWidthChild += Child->Style->Width;
                TotalHeightChild += Child->Style->Height;
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
                            NewWidth = Child->Style->Width * TotalWidth / TotalWidthChild;
                            XIteration += NewWidth; 
                        }else{
                            NewWidth = TotalWidth;
                        }
                        Child->UpdateFramebuffer(NewWidth, Child->Style->Height);
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
                        if(IterateX) XIteration += SpaceFreePerChild + Child->Style->Width;
                    }

                    if(IterateX) XIteration += (TotalWidth - TotalWidthChild) % (Cpnt->Childs->length - 1);

                    for(; i < Cpnt->Childs->length; i++) {
                        Component* Child = (Component*)vector_get(Cpnt->Childs, i);
                        Child->Style->Position.x = XIteration;
                        if(IterateX) XIteration += SpaceFreePerChild + Child->Style->Width;
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
                        if(IterateX) XIteration += SpaceFreePerChild + Child->Style->Width;
                    }

                    if(IterateX) XIteration += (TotalWidth - TotalWidthChild) % Cpnt->Childs->length;

                    for(; i < Cpnt->Childs->length; i++) {
                        Component* Child = (Component*)vector_get(Cpnt->Childs, i);
                        Child->Style->Position.x = XIteration;
                        if(IterateX) XIteration += SpaceFreePerChild + Child->Style->Width;
                    }
                    break;
                }
                case Ui::Layout::LEFT:{
                    uint64_t XIteration = 0;
                    for(uint64_t i = 0; i < Cpnt->Childs->length; i++) {
                        Component* Child = (Component*)vector_get(Cpnt->Childs, i);
                        Child->Style->Position.x = XIteration;
                        XIteration += Child->Style->Width;
                    }
                    break;
                }
                case Ui::Layout::CENTER:{
                    uint64_t XIteration = (TotalWidth - TotalWidthChild) / 2;
                    for(uint64_t i = 0; i < Cpnt->Childs->length; i++) {
                        Component* Child = (Component*)vector_get(Cpnt->Childs, i);
                        Child->Style->Position.x = XIteration;
                        XIteration += Child->Style->Width;
                    }
                    break;
                }
                case Ui::Layout::RIGHT:{
                    uint64_t XIteration = TotalWidth - TotalWidthChild;
                    for(uint64_t i = 0; i < Cpnt->Childs->length; i++) {
                        Component* Child = (Component*)vector_get(Cpnt->Childs, i);
                        Child->Style->Position.x = XIteration;
                        XIteration += Child->Style->Width;
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
                            NewHeight = Child->Style->Height * TotalHeight / TotalHeightChild;
                            YIteration += NewHeight;
                        }else{
                            NewHeight = TotalHeight;
                        }
                        Child->UpdateFramebuffer(Child->Style->Width, NewHeight);
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
                        if(IterateY) YIteration += SpaceFreePerChild + Child->Style->Height;
                        Child->UpdateFunction(Child);
                    }

                    if(IterateY) YIteration += (TotalHeight - TotalHeightChild) % (Cpnt->Childs->length - 1);

                    for(; i < Cpnt->Childs->length; i++) {
                        Component* Child = (Component*)vector_get(Cpnt->Childs, i);
                        Child->Style->Position.y = YIteration;
                        if(IterateY) YIteration += SpaceFreePerChild + Child->Style->Height;
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
                        if(IterateY) YIteration += SpaceFreePerChild + Child->Style->Height;
                        Child->UpdateFunction(Child);
                    }

                    if(IterateY) YIteration += (TotalHeight - TotalHeightChild) % Cpnt->Childs->length;

                    for(; i < Cpnt->Childs->length; i++) {
                        Component* Child = (Component*)vector_get(Cpnt->Childs, i);
                        Child->Style->Position.y = YIteration;
                        if(IterateY) YIteration += SpaceFreePerChild + Child->Style->Height;
                        Child->UpdateFunction(Child);
                    }
                    break;
                }
                case Ui::Layout::TOP:{
                    uint64_t YIteration = 0;
                    for(uint64_t i = 0; i < Cpnt->Childs->length; i++) {
                        Component* Child = (Component*)vector_get(Cpnt->Childs, i);
                        Child->Style->Position.y = YIteration;
                        YIteration += Child->Style->Height;
                        Child->UpdateFunction(Child);
                    }
                    break;
                }
                case Ui::Layout::MIDDLE:{
                    uint64_t YIteration = (TotalHeight - TotalHeightChild) / 2;
                    for(uint64_t i = 0; i < Cpnt->Childs->length; i++) {
                        Component* Child = (Component*)vector_get(Cpnt->Childs, i);
                        Child->Style->Position.y = YIteration;
                        YIteration += Child->Style->Height;
                        Child->UpdateFunction(Child);
                    }
                    break;
                }
                case Ui::Layout::BOTTOM:{
                    uint64_t YIteration = 0;
                    for(uint64_t i = 0; i < Cpnt->Childs->length; i++) {
                        Component* Child = (Component*)vector_get(Cpnt->Childs, i);
                        Child->Style->Position.y = TotalHeight - Child->Style->Height;
                        YIteration += Child->Style->Height;
                        Child->UpdateFunction(Child);
                    }
                    break;
                }
            }
        }
        BlitFramebuffer(Cpnt->Parent->GetFramebuffer(), Cpnt->GetFramebuffer(), Cpnt->Style->Position.x, Cpnt->Style->Position.y);
    }

    Flexbox_t* Flexbox(FlexboxStyle_t Style, UiContext* ParentUiContex){
        Flexbox_t* Flexbox = (Flexbox_t*)malloc(sizeof(Flexbox_t));
        memcpy(&Flexbox->Style, &Style, sizeof(FlexboxStyle_t));
        Flexbox->Cpnt = new Component({ .Width = Style.Width, .Height = Style.Height, .IsVisible = Style.IsVisible}, FlexboxUpdate, NULL, (uintptr_t)Flexbox, ParentUiContex, false);
        return Flexbox;
    }

}