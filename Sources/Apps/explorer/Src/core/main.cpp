#include <core/main.h>

extern "C" int main() {
    UiWindow::Window* ExplorerWindow = new UiWindow::Window("File explorer", "explorer.tga", 600, 600, 600, 10);
    
    directory_t* Dir = opendir("d1:");
    directory_entries_t* Directories = mreaddir(Dir, 0, 0xff);
    directory_entry_t* Entry = &Directories->FirstEntry;
    for(uint64_t i = 0; i < Directories->EntryCount; i++){
        Ui::Box_t* Filebox = Box( 
            { 
                .G = { 
                        .Width = 100,
                        .Height = 100,
                        .IsHidden = false
                    }, 
                .BackgroundColor = 0x1E1E1E,
                .HoverColor = 0xff0000
            }
        , ExplorerWindow->Cpnt);

        Ui::Label_t* FileName = Ui::Label({
            .Text = Entry->Name,
            .FontSize = 12,
            .ForegroundColor = 0xffffffff,
            .Align = Ui::TEXTALIGNCENTER,
            .AutoWidth = false,
            .AutoHeight = true,
            .G{
                .Width = -100,
                .Align{
                    .x = Ui::AlignTypeX::CENTER,
                    .y = Ui::AlignTypeY::MIDDLE,
                },     
            }
        }, Filebox->Cpnt);

        Entry = (directory_entry_t*)((uint64_t)&Directories->FirstEntry + Entry->NextEntryPosition);
    }

    return KSUCCESS;
}