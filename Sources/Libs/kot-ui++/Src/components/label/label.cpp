#include <kot-ui++/component.h>
#include <kot/uisd/srvs/storage.h>

#include <kot/stdio.h>

namespace Ui {
    void LabelDraw(Label_t* Label){
        switch(Label->Style.Align){
            case TEXTALIGNLEFT:{
                EditPen(Label->Font, NULL, (int64_t)(Label->Cpnt->FramebufferRelativePosition.x + Label->Cpnt->Style->Margin.Left), (int64_t)(Label->Cpnt->FramebufferRelativePosition.y + Label->Cpnt->Style->Margin.Top), -1, -1, -1);
                break;
            }
            case TEXTALIGNCENTER:{
                EditPen(Label->Font, NULL, (int64_t)(Label->Cpnt->FramebufferRelativePosition.x + Label->Cpnt->Style->Margin.Left + (Label->Cpnt->Style->Currentwidth - Label->TextWidth) / 2), (int64_t)(Label->Cpnt->FramebufferRelativePosition.y + Label->Cpnt->Style->Margin.Top), -1, -1, -1);
                break;
            }
            case TEXTALIGNRIGHT:{
                EditPen(Label->Font, NULL, (int64_t)(Label->Cpnt->FramebufferRelativePosition.x + Label->Cpnt->Style->Margin.Left + Label->Cpnt->Style->Currentwidth - Label->TextWidth), (int64_t)(Label->Cpnt->FramebufferRelativePosition.y + Label->Cpnt->Style->Margin.Top), -1, -1, -1);
                break;
            }
            default:{
                // TEXTALIGNLEFT
                EditPen(Label->Font, NULL, (int64_t)(Label->Cpnt->FramebufferRelativePosition.x + Label->Cpnt->Style->Margin.Left), (int64_t)(Label->Cpnt->FramebufferRelativePosition.y + Label->Cpnt->Style->Margin.Top), -1, -1, -1);
                break;
            }
        }

        Label->Cpnt->DrawPosition.x = Label->Cpnt->FramebufferRelativePosition.x; // Do not use real position
        Label->Cpnt->DrawPosition.y = Label->Cpnt->FramebufferRelativePosition.y;
        DrawFont(Label->Font, Label->Style.Text);
        Label->Cpnt->IsRedraw = true;
    }

    void LabelUpdate(Component* Cpnt){
        Label_t* Label = (Label_t*)Cpnt->ExternalData;
        Cpnt->AbsolutePosition = {.x = (int64_t)(Cpnt->Parent->AbsolutePosition.x + Cpnt->Style->Position.x + Cpnt->Style->Margin.Left), .y = (int64_t)(Cpnt->Parent->AbsolutePosition.y + Cpnt->Style->Position.y + Cpnt->Style->Margin.Top)};
        Cpnt->FramebufferRelativePosition = {.x = Cpnt->Parent->FramebufferRelativePosition.x + Cpnt->Style->Position.x, .y = Cpnt->Parent->FramebufferRelativePosition.y + Cpnt->Style->Position.y};
        if(Cpnt->IsFramebufferUpdate){
            // Draw
            Cpnt->IsFramebufferUpdate = false;
            font_fb_t FontBuffer = {.Address = Label->Cpnt->Framebuffer->Buffer, .Width = Label->Cpnt->Parent->Framebuffer->Width, .Height = Label->Cpnt->Parent->Framebuffer->Height, .Pitch = Label->Cpnt->Parent->Framebuffer->Pitch};
            EditPen(Label->Font, &FontBuffer, (int64_t)(Cpnt->Style->Position.x + Cpnt->Style->Margin.Left), (int64_t)(Cpnt->Style->Position.y + Cpnt->Style->Margin.Top), Label->Style.FontSize, 0, Label->Style.ForegroundColor);
            // TODO add buffer
        }else if(Label->IsDrawUpdate){
            Label->IsDrawUpdate = false;
            // TODO add buffer
        }else if(Cpnt->Parent->IsRedraw || Cpnt->DrawPosition.x != Cpnt->FramebufferRelativePosition.x || Cpnt->DrawPosition.y != Cpnt->FramebufferRelativePosition.y){
            // TODO add buffer
        }
        // TODO optimize
        LabelDraw(Label);
        Cpnt->Update();
    }

    Label_t* Label(LabelStyle_t Style, Component* ParentCpnt){
        if(Style.FontPath == NULL){
            return NULL;
        }
        // Load font
        file_t* FontFile = fopen(Style.FontPath, "rb");

        if(FontFile == NULL){
            return NULL;
        }

        fseek(FontFile, 0, SEEK_END);
        size_t FontFileSize = ftell(FontFile);
        fseek(FontFile, 0, SEEK_SET);
        uintptr_t Font = malloc(FontFileSize);
        fread(Font, FontFileSize, 1, FontFile);
        Label_t* Label = (Label_t*)malloc(sizeof(Label_t));
        Label->Font = LoadFont(Font);
        fclose(FontFile);

        memcpy(&Label->Style, &Style, sizeof(LabelStyle_t));
        size_t Lenght = strlen(Style.Text);
        Label->Style.Text = (char*)malloc(Lenght + 1);
        strcpy(Label->Style.Text, Style.Text);
        Label->Style.Text[Lenght] = 0;
        
        Label->Cpnt = new Component(Style.G, LabelUpdate, (Ui::MouseEventHandler)LabelUpdate, (uintptr_t)Label, ParentCpnt, false);

        font_fb_t FontBuffer = {.Address = Label->Cpnt->Framebuffer->Buffer, .Width = Label->Cpnt->Parent->Framebuffer->Width, .Height = Label->Cpnt->Parent->Framebuffer->Height, .Pitch = Label->Cpnt->Parent->Framebuffer->Pitch};
        LoadPen(Label->Font, &FontBuffer, (int64_t)(Label->Cpnt->Style->Position.x + Label->Cpnt->Style->Margin.Left), (int64_t)(Label->Cpnt->Style->Position.y + Label->Cpnt->Style->Margin.Top), Label->Style.FontSize, 0, Label->Style.ForegroundColor);
        GetTextboxInfo(Label->Font, Label->Style.Text, &Label->TextWidth, &Label->TextHeight, &Label->TextX, &Label->TextY);
        
        if(Label->Style.AutoWidth){
            Label->Cpnt->Style->Width = Label->TextWidth;
            Label->Cpnt->UpdateFramebuffer(Label->TextWidth, Label->Cpnt->Style->Currentheight);
        }
        if(Label->Style.AutoHeight){
            Label->Cpnt->Style->Height = Label->TextHeight;
            Label->Cpnt->UpdateFramebuffer(Label->Cpnt->Style->Currentwidth, Label->TextHeight);
        }
        Label->IsDrawUpdate = true;
        return Label;
    }

    void Label_t::UpdateText(char* Text){
        uintptr_t OldText = (uintptr_t)Style.Text;
        size_t Lenght = strlen(Style.Text);
        this->Style.Text = (char*)malloc(Lenght + 1);
        strcpy(this->Style.Text, Style.Text);
        this->Style.Text[Lenght] = 0;
        GetTextboxInfo(this->Font, this->Style.Text, &this->TextWidth, &this->TextHeight, &this->TextX, &this->TextY);
        if(this->Style.AutoWidth){
            this->Cpnt->Style->Width = this->TextWidth;
        }
        if(this->Style.AutoHeight){
            this->Cpnt->Style->Height = this->TextHeight;
        }
        IsDrawUpdate = true;
        free(OldText);
    }

}