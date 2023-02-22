#include <kot-ui++/component.h>
#include <kot/uisd/srvs/storage.h>

namespace Ui {
    void LabelDraw(Label_t* Label){
        EditPen(Label->Font, NULL, Label->Cpnt->Style->Position.x, Label->Cpnt->Style->Position.y, -1, -1, -1);
        Label->Cpnt->DrawPosition.x = Label->Cpnt->Style->Position.x;
        Label->Cpnt->DrawPosition.y = Label->Cpnt->Style->Position.y;
        DrawFont(Label->Font, Label->Style.Text);
    }

    void LabelUpdate(Component* Cpnt){
        Label_t* Label = (Label_t*)Cpnt->ExternalData;
        if(Cpnt->IsFramebufferUpdate){
            // Draw
            Cpnt->UpdateFramebuffer(Label->Cpnt->Parent->Framebuffer);
            Cpnt->IsFramebufferUpdate = false;
            font_fb_t FontBuffer = {.Address = Label->Cpnt->Framebuffer->Buffer, .Width = Label->Cpnt->Framebuffer->Width, .Height = Label->Cpnt->Framebuffer->Height, .Pitch = Label->Cpnt->Framebuffer->Pitch};
            LoadPen(Label->Font, &FontBuffer, Cpnt->Style->Position.x, Cpnt->Style->Position.y, Label->Style.FontSize, 0, Label->Style.ForegroundColor);
        }else if(Label->IsDrawUpdate){
            Label->IsDrawUpdate = false;
            LabelDraw(Label);
        }
        Cpnt->AbsolutePosition = {.x = Cpnt->Parent->AbsolutePosition.x + Cpnt->Style->Position.x, .y = Cpnt->Parent->AbsolutePosition.y + Cpnt->Style->Position.y};
        if(Cpnt->Parent->IsRedraw || Cpnt->DrawPosition.x != Cpnt->Style->Position.x || Cpnt->DrawPosition.y != Cpnt->Style->Position.y){
            Cpnt->IsRedraw = true;
            LabelDraw(Label);
        }
    }

    Label_t* Label(LabelStyle_t Style, Component* ParentCpnt){
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
        Label->Cpnt = new Component(Style.G, LabelUpdate, NULL, (uintptr_t)Label, ParentCpnt, true);
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