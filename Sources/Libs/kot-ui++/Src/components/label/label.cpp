#include <kot-ui++/component.h>
#include <kot/uisd/srvs/storage.h>

namespace Ui {
    void LabelDraw(Label_t* Label){
        EditPen(Label->Font, NULL, 0, 0, -1, -1, -1);
        DrawFont(Label->Font, Label->Style.Text);
    }

    void LabelUpdate(Component* Cpnt){
        Label_t* Label = (Label_t*)Cpnt->ExternalData;
        if(Cpnt->IsFramebufferUpdate){
            // Draw
            Label->Style.Width = Cpnt->GetStyle()->Width;
            Label->Style.Height = Cpnt->GetStyle()->Height;

            Cpnt->UpdateFramebuffer(Label->Cpnt->Parent->Framebuffer);
            Cpnt->IsFramebufferUpdate = false;
            font_fb_t FontBuffer = {.Address = Label->Cpnt->Framebuffer->Buffer, .Width = Label->Cpnt->Framebuffer->Width, .Height = Label->Cpnt->Framebuffer->Height, .Pitch = Label->Cpnt->Framebuffer->Pitch};
            LoadPen(Label->Font, &FontBuffer, 0, 0, Label->Style.FontSize, 0, Label->Style.ForegroundColor);
        }else if(Label->IsDrawUpdate){
            Label->IsDrawUpdate = false;
        }
        Cpnt->AbsolutePosition = {.x = Cpnt->Parent->AbsolutePosition.x + Cpnt->Style->Position.x, .y = Cpnt->Parent->AbsolutePosition.y + Cpnt->Style->Position.y};
        LabelDraw(Label);
    }

    Label_t* Label(LabelStyle_t Style, UiContext* ParentUiContex){
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
        Label->Cpnt = new Component({ .Width = Style.Width, .Height = Style.Height, .IsVisible = Style.IsVisible, .Position = {.x = Style.Position.x, .y = Style.Position.y}}, LabelUpdate, NULL, (uintptr_t)Label, ParentUiContex, true);
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