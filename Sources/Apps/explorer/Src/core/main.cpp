#include <core/main.h>


UiWindow::Window* ExplorerWindow;

bool IsRoot;
char* MainPath;

void LoadFiles(char* Path);

void FileButton(Ui::Button_t* Button, Ui::ButtonEvent_t Type){
    if(Type & BUTTON_EVENT_TYPE_LEFT_CLICK){
        Explorer_File_Data* FileData = (Explorer_File_Data*)Button->Style.ExternalData;
        if(!FileData->IsFile){
            std::StringBuilder* builder = new std::StringBuilder(MainPath);
            if(IsRoot){
                IsRoot = false;
            }else{
                builder->append("/");
            }
            builder->append(FileData->Name);
            LoadFiles(builder->toString());
            free(builder);
        }
    }
}

void LoadFiles(char* Path){
    ExplorerWindow->Cpnt->ClearChilds();
    directory_t* Dir = opendir(Path);
    MainPath = Path;
    uint64_t FilesCount;
    filecount(Dir, &FilesCount);
    directory_entries_t* Directories = mreaddir(Dir, 0, FilesCount);
    directory_entry_t* Entry = &Directories->FirstEntry;
    for(uint64_t i = 0; i < Directories->EntryCount; i++){
        Explorer_File_Data* FileData = (Explorer_File_Data*)malloc(sizeof(Explorer_File_Data));
        FileData->IsFile = Entry->IsFile;
        FileData->Name = (char*)malloc(strlen(Entry->Name));
        strcpy(FileData->Name, Entry->Name);
        Ui::Button_t* Filebox = Button( 
            { 
                .G = { 
                        .Width = 100,
                        .Height = 100,
                        .IsHidden = false
                    }, 
                .BackgroundColor = 0x1E1E1E,
                .OnMouseEvent = FileButton,
                .ExternalData = (uint64_t)FileData,
                .HoverColor = 0xff0000
            }
        , ExplorerWindow->Cpnt);

        Ui::Picturebox_t* FileImage = Picturebox((char*)(FileData->IsFile ? "file.tga" : "folder.tga"), Ui::_TGA, 
        {
            .Fit = Ui::PICTUREFILL,
            .Transparency = true,
            .G{
                .Width = -100, 
                .Height = -100, 
                .IsHidden = false
            }
        }
        , Filebox->Cpnt);

        Ui::Label_t* LabelFileName = Ui::Label({
            .Text = FileData->Name,
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

    free(Directories);
}

extern "C" int main() {
    ExplorerWindow = new UiWindow::Window("File explorer", "explorer.tga", 600, 600, 600, 10);
    IsRoot = true;
    LoadFiles("d1:");

    return KSUCCESS;
}