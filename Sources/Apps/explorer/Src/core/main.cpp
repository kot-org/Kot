#include <core/main.h>

#define FIELD_WIDTH 90
#define FIELD_HEIGHT 50

#define ROOT_PATH "d1:"
#define ROOT_PATH_LEN strlen(ROOT_PATH)

DIR* Directory;

static char PathBarBuffer[PATH_MAX];

char* NextPath(char* Current, char* Target){
    char* NextPath = (char*)malloc(strlen(Current) + strlen((char*)Target) + strlen("/") + 1);
    NextPath[0] = '\0';
    strcat(NextPath, (char*)Current);
    if((uintptr_t)strchr(Current, ':') != ((uintptr_t)Current + strlen(Current)) - 1){
        strcat(NextPath, "/");
    }
    strcat(NextPath, Target);
    return NextPath;
}

char* LastPath(char* Current){
    char* LastDir = strrchr(Current, '/');
    if(LastDir == NULL){
        char* LastDir = strchr(Current, ':');
        if((uintptr_t)LastDir == ((uintptr_t)Current + strlen(Current)) - 1){
            return NULL;
        }
        size_t Len = (uintptr_t)LastDir - (uintptr_t)Current + 1;
        char* LastPath = (char*)malloc(Len + 1);
        memcpy(LastPath, Current, Len);
        LastPath[Len] = '\0';
        return LastPath;
    }
    size_t Len = (uintptr_t)LastDir - (uintptr_t)Current;
    char* LastPath = (char*)malloc(Len + 1);
    memcpy(LastPath, Current, Len);
    LastPath[Len] = '\0';
    return LastPath;
}

void DrawHeader(kui_Context* Ctx){
    kui_Rect R = kui_get_current_container(Ctx)->body;
    kui_layout_row(Ctx, 5, (int[]){25, 25, R.w - 126, 25, 25}, 27);
    if(kui_button_ex(Ctx, NULL, 1462, 0)){
        char* TmpPath = LastPath((char*)Ctx->opaque);
        if(TmpPath){
            DIR* Tmp = opendir(TmpPath);
            if(Tmp){
                free(Ctx->opaque);
                closedir(Directory);
                Directory = Tmp;
                Ctx->opaque = (void*)TmpPath;
                memcpy(PathBarBuffer, TmpPath, strlen(TmpPath) + 1);
            }else{
                closedir(Tmp);
            }
        }
    }
    if(kui_button_ex(Ctx, NULL, 1463, 0)){
        char* TmpPath = LastPath((char*)Ctx->opaque);
        if(TmpPath){
            DIR* Tmp = opendir(TmpPath);
            if(Tmp){
                free(Ctx->opaque);
                closedir(Directory);
                Directory = Tmp;
                Ctx->opaque = (void*)TmpPath;
                memcpy(PathBarBuffer, TmpPath, strlen(TmpPath) + 1);
            }else{
                closedir(Tmp);
            }
        }
    }

    if(kui_textbox(Ctx, PathBarBuffer, sizeof(PathBarBuffer)) & KUI_RES_SUBMIT){
        size_t Len = strlen(PathBarBuffer);
        if(Len){
            kui_set_focus(Ctx, Ctx->last_id);
            DIR* Tmp = opendir(PathBarBuffer);
            if(Tmp){
                free(Ctx->opaque);
                closedir(Directory);
                Directory = Tmp;
                char* TmpPath = (char*)malloc(Len + 1);
                memcpy(TmpPath, PathBarBuffer, Len);
                TmpPath[Len] = '\0';
                Ctx->opaque = (void*)TmpPath;
            }else{
                closedir(Tmp);
            }
        }
    }
    
    if(kui_button_ex(Ctx, NULL, 873, 0)){
        char* TmpPath = LastPath((char*)Ctx->opaque);
        if(TmpPath){
            DIR* Tmp = opendir(TmpPath);
            if(Tmp){
                free(Ctx->opaque);
                closedir(Directory);
                Directory = Tmp;
                Ctx->opaque = (void*)TmpPath;
                memcpy(PathBarBuffer, TmpPath, strlen(TmpPath) + 1);
            }else{
                closedir(Tmp);
            }
        }
    }
    if(kui_button_ex(Ctx, NULL, 874, 0)){
        char* TmpPath = LastPath((char*)Ctx->opaque);
        if(TmpPath){
            DIR* Tmp = opendir(TmpPath);
            if(Tmp){
                free(Ctx->opaque);
                closedir(Directory);
                Directory = Tmp;
                Ctx->opaque = (void*)TmpPath;
                memcpy(PathBarBuffer, TmpPath, strlen(TmpPath) + 1);
            }else{
                closedir(Tmp);
            }
        }
    }
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

        DrawHeader(Ctx);

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
                    closedir(Directory);
                    Directory = Tmp;
                    Ctx->opaque = (void*)TmpPath;
                    memcpy(PathBarBuffer, TmpPath, strlen(TmpPath) + 1);
                }else{
                    closedir(Tmp);
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
    char* Root = (char*)malloc(ROOT_PATH_LEN + 1);
    memcpy(Root, ROOT_PATH, ROOT_PATH_LEN + 1);
    memcpy(PathBarBuffer, Root, ROOT_PATH_LEN + 1);
    Directory = opendir(Root);
    kui_init(WindowRenderer, (void*)Root);
    return KSUCCESS;
}