#include <display/display.h>

#include <kot++/printf.h>

Label_t* NumberDisplayLabel;

char* NumericKeypadStr[6][4] = {
    {"%",   "CE",   "C",   "<-"},
    {"√x",  "x²",   "1/x",  "/"},
    {"7",   "8",    "9",    "*"},
    {"4",   "5",    "6",    "-"},
    {"1",   "2",    "3",    "+"},
    {"+/-", "0",    ".",    "="},
};

char NumericKeypadData[6][4] = {
    {PERCENT_OP,    CLEARALL_ACTION,    CLEAR_ACTION,       BACKSPACE_ACTION},
    {SQRT_OP,       SQUARE_OP,          RESIPROCAL_OP,      DIV_OP},
    {'7',           '8',                '9',                MULT_OP},
    {'4',           '5',                '6',                SUB_OP},
    {'1',           '2',                '3',                ADD_OP},
    {INVERTSIGN_OP, '0',                DECIMAL_ACTION,     EQUAL_ACTION},
};

/* 
    0 : Background color
    1 : Hover color
    2 : Click color
 */
uint32_t NumericKeypadColor[6][4][3] = {
    {{0x676767, 0x858585, 0x454545},  {0x676767, 0x858585, 0x454545},   {0x676767, 0x858585, 0x454545},   {0x676767, 0x858585, 0x454545}},
    {{0x676767, 0x858585, 0x454545},  {0x676767, 0x858585, 0x454545},   {0x676767, 0x858585, 0x454545},   {0x676767, 0x858585, 0x454545}},
    {{0x676767, 0x858585, 0x454545},  {0x676767, 0x858585, 0x454545},   {0x676767, 0x858585, 0x454545},   {0x676767, 0x858585, 0x454545}},
    {{0x676767, 0x858585, 0x454545},  {0x676767, 0x858585, 0x454545},   {0x676767, 0x858585, 0x454545},   {0x676767, 0x858585, 0x454545}},
    {{0x676767, 0x858585, 0x454545},  {0x676767, 0x858585, 0x454545},   {0x676767, 0x858585, 0x454545},   {0x676767, 0x858585, 0x454545}},
    {{0x676767, 0x858585, 0x454545},  {0x676767, 0x858585, 0x454545},   {0x676767, 0x858585, 0x454545},   {0x0B1AFF, 0x3399FF, 0x0C4BDD}},
};

void OpButton(Button_t* Button, ButtonStatus_t Type) {
    
}

void CreateDisplay(Component* Window) {
    Flexbox_t* Main = Flexbox(  
        {   
            .G = { 
                    .Width = -100,
                    .Height = -100,
                    .IsHidden = false 
                }, 
            .Direction = Layout::COLUMN,
            .Align = { .x = Layout::FILLHORIZONTAL, .y = Layout::FILLVERTICAL }
        }
    , Window);

    Box_t* NumberDisplay = Box( 
        { 
            .G = { 
                    .Width = -100,
                    .Height = 150,
                    .BorderRadius = 20,
                    .IsHidden = false
                }, 

            .BackgroundColor = WIN_DEFAULT_BKGCOLOR
        }
    , Main->Cpnt);

    NumberDisplayLabel = Label(
        {
            .Text = "0",
            .FontSize = 36,
            .ForegroundColor = 0xffffffff,
            .Align = TextAlign::TEXTALIGNRIGHT,
            .AutoWidth = false,
            .AutoHeight = true,

            .G = {
                    .Width = -100,
                    .Align{
                        .x = AlignTypeX::CENTER,
                        .y = AlignTypeY::MIDDLE,
                    },
                    .Margin{
                        .Right = 20,
                    },
                    .AutoPosition = true,        
                }
        }
    , NumberDisplay->Cpnt);

    Gridbox_t* NumericKeypad = Gridbox( 
        { 
            .G = { 
                    .Width = -100,
                    .Height = -100,
                    .IsHidden = false
                },

            .CaseWidth = -4,
            .CaseHeight = -6,
            .SpaceBetweenCaseHorizontal = 5,
            .SpaceBetweenCaseVertical = 5,
        }
    , Main->Cpnt);

    // Load font
    file_t* FontFile = fopen("d0:default-font.sfn", "rb");
    assert(FontFile != NULL);

    fseek(FontFile, 0, SEEK_END);
    size_t FontFileSize = ftell(FontFile);
    fseek(FontFile, 0, SEEK_SET);
    uintptr_t Font = malloc(FontFileSize);
    fread(Font, FontFileSize, 1, FontFile);

    /* Draw the grid with buttons */
    for(uint8_t y = 0; y < 6; y++) {
        for(uint8_t x = 0; x < 4; x++) {
            
            Button_t* Btn = Button(OpButton,
                { 
                    .G = { 
                            .Width = -100,
                            .Height = -100,
                            .BorderRadius = 20,
                            .IsHidden = false
                        },

                    .BackgroundColor = (color_t)NumericKeypadColor[y][x][0], 
                    .HoverColor = (color_t)NumericKeypadColor[y][x][1], 
                    .ClickColor = (color_t)NumericKeypadColor[y][x][2],
                }
            , NumericKeypad->Cpnt);

            Label_t* BtnLabel = Label(
                {
                    .Text = NumericKeypadStr[y][x],
                    .FontSize = 16,
                    .FontBuffer = Font,
                    .ForegroundColor = 0xffffffff,
                    .Align = TextAlign::TEXTALIGNCENTER,
                    .AutoWidth = false,
                    .AutoHeight = true,

                    .G = {
                        .Height = -100,
                        .Width = -100,
                        .Align = {
                            .x = AlignTypeX::CENTER,
                            .y = AlignTypeY::MIDDLE,
                        },
                        .AutoPosition = true,        
                    }
                }
            , Btn->Cpnt);

        }
    }
    fclose(FontFile);
}

void DisplayNumber(int64_t Number) {
    char Buffer[50];

    NumberDisplayLabel->UpdateText(itoa(Number, Buffer, 10)); 
}