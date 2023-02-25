#include <kot-ui++/component.h>
#include <kot/uisd/srvs/storage.h>

namespace Ui {
    void LabelDraw(Label_t* Label){
        Label->Cpnt->IsRedraw = true;
        EditPen(Label->Font, NULL, (int64_t)(Label->Cpnt->Style->Position.x + Label->Cpnt->Style->Margin.Left), (int64_t)(Label->Cpnt->Style->Position.y + Label->Cpnt->Style->Margin.Top), -1, -1, -1);
        Label->Cpnt->DrawPosition.x = Label->Cpnt->Style->Position.x; // Do not use real position
        Label->Cpnt->DrawPosition.y = Label->Cpnt->Style->Position.y;
        DrawFont(Label->Font, Label->Style.Text);
    }

    void LabelUpdate(Component* Cpnt){
        Label_t* Label = (Label_t*)Cpnt->ExternalData;
        if(Cpnt->IsFramebufferUpdate){
            // Draw
            Cpnt->IsFramebufferUpdate = false;
            font_fb_t FontBuffer = {.Address = Label->Cpnt->Framebuffer->Buffer, .Width = Label->Cpnt->Framebuffer->Width, .Height = Label->Cpnt->Framebuffer->Height, .Pitch = Label->Cpnt->Framebuffer->Pitch};
            EditPen(Label->Font, &FontBuffer, (int64_t)(Cpnt->Style->Position.x + Cpnt->Style->Margin.Left), (int64_t)(Cpnt->Style->Position.y + Cpnt->Style->Margin.Top), Label->Style.FontSize, 0, Label->Style.ForegroundColor);
            LabelDraw(Label);
        }else if(Label->IsDrawUpdate){
            Label->IsDrawUpdate = false;
            LabelDraw(Label);
        }else if(Cpnt->Parent->IsRedraw || Cpnt->DrawPosition.x != Cpnt->Style->Position.x || Cpnt->DrawPosition.y != Cpnt->Style->Position.y){
            Cpnt->IsRedraw = true;
            LabelDraw(Label);
        }

        Cpnt->AbsolutePosition = {.x = (int64_t)(Cpnt->Parent->AbsolutePosition.x + Cpnt->Style->Position.x + Cpnt->Style->Margin.Left), .y = (int64_t)(Cpnt->Parent->AbsolutePosition.y + Cpnt->Style->Position.y + Cpnt->Style->Margin.Top)};
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
        Label->Style.Text = (char*)malloc(strlen(Style.Text));
        strcpy(Label->Style.Text, Style.Text);

        Label->Cpnt = new Component(Style.G, LabelUpdate, (Ui::MouseEventHandler)LabelUpdate, (uintptr_t)Label, ParentCpnt, false);
        
        font_fb_t FontBuffer = {.Address = Label->Cpnt->Framebuffer->Buffer, .Width = Label->Cpnt->Framebuffer->Width, .Height = Label->Cpnt->Framebuffer->Height, .Pitch = Label->Cpnt->Framebuffer->Pitch};
        LoadPen(Label->Font, &FontBuffer, (int64_t)(Label->Cpnt->Style->Position.x + Label->Cpnt->Style->Margin.Left), (int64_t)(Label->Cpnt->Style->Position.y + Label->Cpnt->Style->Margin.Top), Label->Style.FontSize, 0, Label->Style.ForegroundColor);
        return Label;
    }

    void Label_t::UpdateText(char* Text){
        uintptr_t OldText = (uintptr_t)Style.Text;
        Style.Text = (char*)malloc(strlen(Style.Text));
        strcpy(Style.Text, Style.Text);
        IsDrawUpdate = true;
        free(OldText);
    }

}