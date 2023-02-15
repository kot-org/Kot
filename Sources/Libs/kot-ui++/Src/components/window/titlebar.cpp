#include <kot-ui++/component.h>

namespace Ui {
    void TitlebarDraw(Component* Cpnt){
        Titlebar_t* Titlebar = (Titlebar_t*)Cpnt->ExternalData;
        memset32(Titlebar->Cpnt->GetFramebuffer()->Buffer, Titlebar->Style.BackgroundColor, Titlebar->Cpnt->GetFramebuffer()->Size);
    }

    Titlebar_t* Titlebar(char* Title, TitlebarStyle_t Style, UiContext* ParentUiContex) {
        /* todo: drag region */
        /* todo: replace Width: 500 -> 100% */
        Titlebar_t* Titlebar = (Titlebar_t*)malloc(sizeof(Titlebar_t));
        memcpy(&Titlebar->Style, &Style, sizeof(TitlebarStyle_t));
        Titlebar->Cpnt = UiLayout::Flexbox({ .Direction = Layout::HORIZONTAL, .Space = Layout::BETWEEN, .Width = 500}, ParentUiContex);
        Titlebar->Cpnt->ExternalData = Titlebar;
        
        //auto windowInfo = Ui::Box({ .Width = Titlebar->Cpnt->GetStyle()->Width/2, .Height = 40, .Color = 0xFF0000 }, ParentUiContex);
        
        for(uint64_t i = 0; i < 25; i++){
            auto iconBox = Ui::Box({ .Width = 20, .Height = 20, .Color = (uint32_t)(0xff << i) }, ParentUiContex);
            Titlebar->Cpnt->AddChild(iconBox->Cpnt); 
        }
        TitlebarDraw(Titlebar->Cpnt);
        // auto titleBox = Ui::Box({ .Width = 40, .Height = 25, .Color = 0xFF00FF }, ParentUiContex);
        // windowInfo->Cpnt->AddChild(titleBox->Cpnt); 
        

        // auto buttons = Ui::Box({ .Height = 25, .Width = 60, .Color = 0xffffff }, ParentUiContex);
        //Titlebar->Cpnt->AddChild(windowInfo->Cpnt);

        /* auto box2box = Ui::box({ .Width = 10, .Height = 10, .color = 0xFFFF00 });
        buttons->addChild(box2box);  */
        
        // Titlebar->Cpnt->AddChild(buttons->Cpnt);

        //TitlebarDraw(Titlebar->Cpnt);

        return Titlebar;
    }

}