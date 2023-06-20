#include <core/main.h>

#define FIELD_WIDTH 90
#define FIELD_HEIGHT 50

DIR* Directory;

static char PathBuffer[PATH_MAX];

char* NextPath(char* Current, char* Target){
    char* NextPath = (char*)malloc(strlen(Current) + strlen((char*)Target) + strlen("/") + 1);
    NextPath[0] = '\0';
    strcat(NextPath, (char*)Current);
    strcat(NextPath, "/");
    strcat(NextPath, Target);
    return NextPath;
}

char* LastPath(char* Current){
    char* LastSlash = strrchr(Current, '/');
    if(LastSlash == NULL){
        return NULL;
    }
    size_t Len = (uintptr_t)LastSlash - (uintptr_t)Current;
    char* LastPath = (char*)malloc(Len + 1);
    memcpy(LastPath, Current, Len);
    LastPath[Len] = '\0';
    return LastPath;
}

void WindowRenderer(kui_Context* Ctx){
    kui_Container* Cnt;

    kui_begin(Ctx);

    if(kui_begin_window(Ctx, "File explorer", kui_rect(50, 50, 900, 400))){
        Cnt = kui_get_current_container(Ctx);

        uint64_t FieldCount = Cnt->rect.w / FIELD_WIDTH;
        uint64_t FieldWidth = Cnt->rect.w / FieldCount;

        struct dirent* Entry = NULL;
        uint64_t Count = 0;
        kui_layout_row(Ctx, 3, (int[]){25, 25, -1}, 27);
        if(kui_button_ex(Ctx, NULL, 1462, 0)){
            char* TmpPath = LastPath((char*)Ctx->opaque);
            if(TmpPath){
                DIR* Tmp = opendir(TmpPath);
                if(Tmp){
                    free(Ctx->opaque);
                    Directory = Tmp;
                    Ctx->opaque = (void*)TmpPath;
                    memcpy(PathBuffer, TmpPath, strlen(TmpPath) + 1);
                }else{
                    free(Tmp);
                }
            }
        }
        if(kui_button_ex(Ctx, NULL, 1463, 0)){
            char* TmpPath = LastPath((char*)Ctx->opaque);
            if(TmpPath){
                DIR* Tmp = opendir(TmpPath);
                if(Tmp){
                    free(Ctx->opaque);
                    Directory = Tmp;
                    Ctx->opaque = (void*)TmpPath;
                    memcpy(PathBuffer, TmpPath, strlen(TmpPath) + 1);
                }else{
                    free(Tmp);
                }
            }
        }

        if(kui_textbox(Ctx, PathBuffer, sizeof(PathBuffer)) & KUI_RES_SUBMIT){
            kui_set_focus(Ctx, Ctx->last_id);
        }
        kui_layout_row(Ctx, 2, (int[]){150, -1}, -1);
        kui_begin_panel(Ctx, "Bookmarks");
        kui_end_panel(Ctx);
        kui_begin_panel(Ctx, "Files");
        while((Entry = readdir(Directory)) != NULL){
            if(!strcmp(Entry->d_name, ".") || !strcmp(Entry->d_name, "..")){
                continue;
            }
            kui_layout_row(Ctx, 1, (int[]){-1}, 25);
            if(kui_button_ex(Ctx, Entry->d_name, 0, 0)){
                char* TmpPath = NextPath((char*)Ctx->opaque, Entry->d_name);
                DIR* Tmp = opendir(TmpPath);
                if(Tmp){
                    free(Ctx->opaque);
                    Directory = Tmp;
                    Ctx->opaque = (void*)TmpPath;
                    memcpy(PathBuffer, TmpPath, strlen(TmpPath) + 1);
                }else{
                    free(Tmp);
                }
            }
            Count++;
        }
        kui_end_panel(Ctx);
        rewinddir(Directory);

        kui_end_window(Ctx);
    }
    kui_end(Ctx);
    kui_r_present(Cnt);
}

int main(int argc, char* argv[]){
    char* Root = (char*)malloc(strlen("d1:.") + 1);
    memcpy(Root, "d1:.", strlen("d1:.") + 1);
    memcpy(PathBuffer, Root, strlen("d1:.") + 1);
    Directory = opendir(Root);
    kui_init(WindowRenderer, (void*)Root);
    return KSUCCESS;
}