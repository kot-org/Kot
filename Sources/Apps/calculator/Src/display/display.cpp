#include <display/display.h>

#include <kot++/printf.h>

char NumericKeypadStr[][4][4] = {
    {{"%"}, {"√"}, {"x²"}, {"1/x"}},
    {{"CE"}, {"C"}, {"<-"}, {"/"}},
    {{"7"}, {"8"}, {"9"}, {"*"}},
    {{"4"}, {"5"}, {"6"}, {"-"}},
    {{"1"}, {"2"}, {"3"}, {"+"}},
    {{"-"}, {"0"}, {"."}, {"="}},
};


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
                    .Width = 400,
                    .Height = 100,
                    .BorderRadius = 20,
                    .IsHidden = false
                }, 
            .BackgroundColor = 0x1E1E1E
        }
    , Main->Cpnt);

    Label_t* NumberDisplayLabel = Ui::Label({
            .Text = "0",
            .FontSize = 36,
            .ForegroundColor = 0xffffffff,
            .Align = Ui::TEXTALIGNRIGHT,
            .AutoWidth = true,
            .AutoHeight = true,
            .G{
                .Align{
                    .x = AlignTypeX::CENTER,
                    .y = AlignTypeY::MIDDLE,
                },
                .AutoPosition = true,        
            }
        }, NumberDisplay->Cpnt);

    Gridbox_t* NumericKeypad = Gridbox( 
        { 
            .G = { 
                    .Width = -100,
                    .Height = 600,
                    .IsHidden = false
                },
            .CaseWidth = 100,
            .CaseHeight = 70
        }
    , Main->Cpnt);

    for(uint8_t y = 0; y < 6; y++) {
        for(uint8_t x = 0; x < 4; x++) {
            Button_t* Button = Ui::Button(  
                { 
                    .G = { 
                            .Width = 100 - 5,
                            .Height = 70 - 5,
                            .Margin = { .Top = 5, .Left = 5 },
                            .Position = { .x = (int64_t)x, .y = (int64_t)y },
                            .BorderRadius = 20,
                            .IsHidden = false
                        },
                    .BackgroundColor = (color_t)0x676767, 
                    .ClickColor = (color_t)0x0C4BDD, 
                    .HoverColor = (color_t)0x3399FF, 
                    .OnMouseEvent = OpButton
                }
            , NumericKeypad->Cpnt);

            Label_t* Label = Ui::Label({
                .Text = NumericKeypadStr[y][x],
                .FontSize = 16,
                .ForegroundColor = 0xffffffff,
                .Align = Ui::TEXTALIGNCENTER,
                .AutoWidth = false,
                .AutoHeight = true,
                .G{
                    .Height = -100,
                    .Width = -100,
                    .Align{
                        .x = AlignTypeX::CENTER,
                        .y = AlignTypeY::MIDDLE,
                    },
                    .AutoPosition = true,        
                }
            }, Button->Cpnt);
        }
    }
}