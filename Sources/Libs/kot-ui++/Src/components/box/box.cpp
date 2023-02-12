#include <kot-ui++/component.h>

namespace Ui {
    void BoxMouseEvent(class Component* Component, uint64_t RelativePositionX, uint64_t RelativePositionY, uint64_t PositionX, uint64_t PositionY, uint64_t ZValue, uint64_t Status){

    }

    void BoxDraw(Component* Cpnt){
        Box_t* Box = (Box_t*)Cpnt->ExternalData;
        memset32(Box->Cpnt->GetFramebuffer()->Buffer, Box->Style.Color, Box->Cpnt->GetFramebuffer()->Size);
    }

    Box_t* Box(BoxStyle_t Style, UiContext* ParentUiContex){
        Box_t* Box = (Box_t*)malloc(sizeof(Box_t));
        memcpy(&Box->Style, &Style, sizeof(BoxStyle_t));
        Box->Cpnt = new Component({ .Width = Style.Width, .Height = Style.Height}, BoxDraw, BoxMouseEvent, (uintptr_t)Box, ParentUiContex);
        BoxDraw(Box->Cpnt);
        return Box;
    }

}