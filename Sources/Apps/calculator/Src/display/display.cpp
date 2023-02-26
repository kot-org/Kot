#include <display/display.h>

#include <kot++/printf.h>

void OpButton(Button_t* Button, ButtonEvent_t Type) {
    
}

void CreateDisplay(Component* Window) {
    Flexbox_t* Main = Flexbox(  
        {   
            .G = { 
                    .Width = 400,
                    .Height = 600,
                    .IsHidden = false 
                }, 
            .Align = { .x = Layout::FILLHORIZONTAL, .y = Layout::TOP }
        }
    , Window);

    Box_t* NumberDisplay = Box( 
        { 
            .G = { 
                    .Width = 100,
                    .Height = 100,
                    .IsHidden = false
                }, 
            .BackgroundColor = 0xFF0000
        }
    , Main->Cpnt);

    Gridbox_t* NumericKeypad = Gridbox( 
        { 
            .G = { 
                    .Width = -100,
                    .Maxwidth = NO_MAXIMUM,
                    .Height = 400,
                    .IsHidden = false
                },
            .CaseWidth = 100,
            .CaseHeight = 70
        }
    , Main->Cpnt);

    for(uint8_t y = 0; y < 4; y++) {
        for(uint8_t x = 0; x < 4; x++) {
            Button_t* Button = Ui::Button(  
                { 
                    .G = { 
                            .Width = 100 - 5,
                            .Height = 70 - 5,
                            .Margin = { .Top = 5, .Left = 5 },
                            .Position = { .x = (int64_t)x, .y = (int64_t)y },
                            .BorderRadius = 10,
                            .IsHidden = false
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