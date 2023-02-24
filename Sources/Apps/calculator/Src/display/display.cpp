#include <display/display.h>

#include <kot++/printf.h>

void OpButton(Button_t* Button, ButtonEventType Type) {
    
}

void CreateDisplay(Component* Window) {
    Flexbox_t* main = Flexbox(  
        {   
            .G = { 
                    .Width = 400,
                    .Height = 600,
                    .IsVisible = true 
                }, 
            .Align = { .x = Layout::FILLHORIZONTAL, .y = Layout::TOP }
        }
    , Window);

    Box_t* NumberDisplay = Box( 
        { 
            .G = { 
                    .Width = 100,
                    .Height = 100 
                }, 
            .BackgroundColor = 0xFF0000
        }
    , main->Cpnt);

    Gridbox_t* NumericKeypad = Gridbox( 
        { 
            .G = { 
                    .Width = -100,
                    .Height = 400,
                    .IsVisible = true
                },
            .CaseWidth = 100,
            .CaseHeight = 70
        }
    , main->Cpnt);

    for(uint8_t y = 0; y < 4; y++) {
        for(uint8_t x = 0; x < 4; x++) {
            Button_t* Button = Ui::Button(  
                { 
                    .G = { 
                            .Width = 100 - 5,
                            .Height = 70 - 5,
                            .Margin = { .Top = 5, .Left = 5 },
                            .Position = { .x = (int64_t)x, .y = (int64_t)y },
                            .IsVisible = true
                        },
                    .BackgroundColor = (color_t)0xFFFFFF, 
                    .ClickColor = (color_t)0x00FF00, 
                    .HoverColor = (color_t)0xFF0000, 
                    .OnMouseEvent = OpButton
                }
            , NumericKeypad->Cpnt);
        }
    }
}