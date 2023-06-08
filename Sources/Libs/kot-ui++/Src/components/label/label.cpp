#include <kot-ui++/component.h>
#include <string.h>
#include <stdio.h>

namespace Ui {
    void LabelDraw(Label_t* Label) {
        atomicAcquire(&Label->Lock, 0);

        switch(Label->Style.Align)
        {
            case TEXTALIGNLEFT:{
                EditPen(Label->Font, NULL, (int64_t)(Label->Cpnt->FramebufferRelativePosition.x + Label->Cpnt->Style->Margin.Left - Label->Cpnt->Style->Margin.Right), (int64_t)(Label->Cpnt->FramebufferRelativePosition.y + Label->Cpnt->Style->Margin.Top - Label->Cpnt->Style->Margin.Bottom), -1, -1, -1);
                break;
            }
            case TEXTALIGNCENTER:{
                EditPen(Label->Font, NULL, (int64_t)(Label->Cpnt->FramebufferRelativePosition.x + Label->Cpnt->Style->Margin.Left + (Label->Cpnt->Style->Currentwidth - Label->TextWidth) / 2 - Label->Cpnt->Style->Margin.Right), (int64_t)(Label->Cpnt->FramebufferRelativePosition.y + Label->Cpnt->Style->Margin.Top - Label->Cpnt->Style->Margin.Bottom), -1, -1, -1);
                break;
            }
            case TEXTALIGNRIGHT:{
                EditPen(Label->Font, NULL, (int64_t)(Label->Cpnt->FramebufferRelativePosition.x + Label->Cpnt->Style->Margin.Left + Label->Cpnt->Style->Currentwidth - Label->TextWidth - Label->Cpnt->Style->Margin.Right), (int64_t)(Label->Cpnt->FramebufferRelativePosition.y + Label->Cpnt->Style->Margin.Top - Label->Cpnt->Style->Margin.Bottom), -1, -1, -1);
                break;
            }
            default:{
                // TEXTALIGNLEFT
                EditPen(Label->Font, NULL, (int64_t)(Label->Cpnt->FramebufferRelativePosition.x + Label->Cpnt->Style->Margin.Left - Label->Cpnt->Style->Margin.Right), (int64_t)(Label->Cpnt->FramebufferRelativePosition.y + Label->Cpnt->Style->Margin.Top - Label->Cpnt->Style->Margin.Bottom), -1, -1, -1);
                break;
            }
        }

        Label->Cpnt->DrawPosition.x = Label->Cpnt->FramebufferRelativePosition.x; // Do not use real position
        Label->Cpnt->DrawPosition.y = Label->Cpnt->FramebufferRelativePosition.y;

        DrawFont(Label->Font, Label->Style.Text);
        Label->Cpnt->IsRedraw = true;

        atomicUnlock(&Label->Lock, 0);
    }

    void LabelUpdate(Component* Cpnt) {
        Label_t* Label = (Label_t*)Cpnt->ExternalData;

        Cpnt->AbsolutePosition = {.x = (int64_t)(Cpnt->Parent->AbsolutePosition.x + Cpnt->Style->Position.x + Cpnt->Style->Margin.Left - Cpnt->Style->Margin.Right), .y = (int64_t)(Cpnt->Parent->AbsolutePosition.y + Cpnt->Style->Position.y + Cpnt->Style->Margin.Top - Cpnt->Style->Margin.Bottom)};
        Cpnt->FramebufferRelativePosition = {.x = Cpnt->Parent->FramebufferRelativePosition.x + Cpnt->Style->Position.x, .y = Cpnt->Parent->FramebufferRelativePosition.y + Cpnt->Style->Position.y};

        if(Cpnt->IsFramebufferUpdate) {
            // Draw
            Cpnt->IsFramebufferUpdate = false;
            font_fb_t FontBuffer = {.Address = Label->Cpnt->Framebuffer->Buffer, .Width = Label->Cpnt->Parent->Framebuffer->Width, .Height = Label->Cpnt->Parent->Framebuffer->Height, .Pitch = Label->Cpnt->Parent->Framebuffer->Pitch};
            EditPen(Label->Font, &FontBuffer, (int64_t)(Cpnt->Style->Position.x + Cpnt->Style->Margin.Left - Cpnt->Style->Margin.Right), (int64_t)(Cpnt->Style->Position.y + Cpnt->Style->Margin.Top - Cpnt->Style->Margin.Bottom), Label->Style.FontSize, 0, Label->Style.ForegroundColor);
            // TODO add buffer
        } else if(Cpnt->IsDrawUpdate) {
            Cpnt->IsDrawUpdate = false;
            // TODO add buffer
        } else if(Cpnt->Parent->IsRedraw || Cpnt->DrawPosition.x != Cpnt->FramebufferRelativePosition.x || Cpnt->DrawPosition.y != Cpnt->FramebufferRelativePosition.y) {
            // TODO add buffer
        }

        // TODO optimize
        Cpnt->Parent->IsDrawUpdate = true;
        LabelDraw(Label);

        Cpnt->Update();
    }

    Label_t* Label(LabelStyle_t Style, Component* ParentCpnt) {
        Label_t* Label = (Label_t*)malloc(sizeof(Label_t));

        // Reset some values
        Label->Lock = 0;
        Label->TextY = 0;
        Label->TextX = 0;
        Label->TextWidth = 0;
        Label->TextHeight = 0;

        if(!Style.FontBuffer) {
            if(Style.FontPath == NULL) {
                free(Label);
                return NULL;
            }
            // Load font
            FILE* FontFile = fopen(Style.FontPath, "rb");

            if(FontFile == NULL){
                free(Label);
                return NULL;
            }

            fseek(FontFile, 0, SEEK_END);
            size_t FontFileSize = ftell(FontFile);
            fseek(FontFile, 0, SEEK_SET);

            void* Font = malloc(FontFileSize);
            fread(Font, FontFileSize, 1, FontFile);

            Label->Font = LoadFont(Font);

            fclose(FontFile);
        } else {
            Label->Font = LoadFont(Style.FontBuffer);
        }

        memcpy(&Label->Style, &Style, sizeof(LabelStyle_t));

        size_t Lenght = strlen(Style.Text);
        Label->Style.Text = (char*)malloc(Lenght + 1);
        strcpy(Label->Style.Text, Style.Text);
        Label->Style.Text[Lenght] = 0;

        atomicAcquire(&Label->Lock, 0);
        
        Label->Cpnt = new Component(Style.G, LabelUpdate, (Ui::MouseEventHandler)LabelUpdate, (void*)Label, ParentCpnt, false);

        font_fb_t FontBuffer = {.Address = Label->Cpnt->Framebuffer->Buffer, .Width = Label->Cpnt->Parent->Framebuffer->Width, .Height = Label->Cpnt->Parent->Framebuffer->Height, .Pitch = Label->Cpnt->Parent->Framebuffer->Pitch};

        LoadPen(Label->Font, &FontBuffer, (int64_t)(Label->Cpnt->Style->Position.x + Label->Cpnt->Style->Margin.Left), (int64_t)(Label->Cpnt->Style->Position.y + Label->Cpnt->Style->Margin.Top), Label->Style.FontSize, 0, Label->Style.ForegroundColor);
        GetTextboxInfo(Label->Font, Label->Style.Text, &Label->TextWidth, &Label->TextHeight, &Label->TextX, &Label->TextY);
        
        if(Label->Style.AutoWidth) {
            Label->Cpnt->Style->Width = Label->TextWidth;
            Label->Cpnt->UpdateFramebuffer(Label->TextWidth, Label->Cpnt->Style->Currentheight);
        }
        if(Label->Style.AutoHeight) {
            Label->Cpnt->Style->Height = Label->TextHeight;
            Label->Cpnt->UpdateFramebuffer(Label->Cpnt->Style->Currentwidth, Label->TextHeight);
        }

        Label->Cpnt->IsDrawUpdate = true;

        atomicUnlock(&Label->Lock, 0);
        return Label;
    }

    void Label_t::UpdateText(char* Text) {
        atomicAcquire(&Lock, 0);

        void* OldText = (void*)this->Style.Text;
        
        size_t Lenght = strlen(Text);
        this->Style.Text = (char*)malloc(Lenght + 1);
        strcpy(this->Style.Text, Text);
        this->Style.Text[Lenght] = 0;

        GetTextboxInfo(this->Font, this->Style.Text, &this->TextWidth, &this->TextHeight, &this->TextX, &this->TextY);
        
        if(this->Style.AutoWidth) {
            this->Cpnt->Style->Width = this->TextWidth;
            this->Cpnt->UpdateFramebuffer(this->TextWidth, this->Cpnt->Style->Currentheight);
        }
        if(this->Style.AutoHeight) {
            this->Cpnt->Style->Height = this->TextHeight;
            this->Cpnt->UpdateFramebuffer(this->Cpnt->Style->Currentwidth, this->TextHeight);
        }

        Cpnt->IsDrawUpdate = true;

        atomicUnlock(&Lock, 0);
        free(OldText);
    }

}