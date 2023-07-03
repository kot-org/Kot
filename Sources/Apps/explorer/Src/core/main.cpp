#include <core/main.h>

#define FIELD_WIDTH 90
#define FIELD_HEIGHT 50

#define ROOT_PATH "/d1:/"
#define ROOT_PATH_LEN strlen(ROOT_PATH)

#define USER_FOLDER "/d1:/user/root/"

DIR* Directory;
dirent* Entries[256];
uint64_t EntriesCount;

static char PathBarBuffer[PATH_MAX];

void UpdateEntries(){
    EntriesCount = 0;
    dirent* Entry = NULL;
    for(uint64_t i = 0; i < EntriesCount; i++){
        free(Entries[EntriesCount]);
    }
    while((Entry = readdir(Directory)) != NULL){
        if(!strcmp(Entry->d_name, ".") || !strcmp(Entry->d_name, "..")){
            continue;
        }
        Entries[EntriesCount] = (dirent*)malloc(sizeof(dirent));
        memcpy(Entries[EntriesCount], Entry, sizeof(dirent));
        EntriesCount++;
        if(EntriesCount >= 256) break;
    }
}

char* NextPath(char* Current, char* Target){
    size_t Length = strlen(Current);
    char* NextPath = (char*)malloc(Length + strlen((char*)Target) + sizeof('/') + 1);
    NextPath[0] = '\0';
    strcat(NextPath, (char*)Current);
    if(Current[Length - 1] != '/'){
        strcat(NextPath, "/");
    }
    strcat(NextPath, Target);
    return NextPath;
}

char* LastPath(char* Current){
    char* LastDir = strrchr(Current, '/');
    if(LastDir == NULL){
        return NULL;
    }else if(LastDir == strchr(Current, '/')){
        size_t Len = (uintptr_t)LastDir - (uintptr_t)Current + 1;
        char* LastPath = (char*)malloc(Len + 1);
        memcpy(LastPath, Current, Len);
        LastPath[Len] = '\0';
        return LastPath;       
    }else{
        size_t Len = (uintptr_t)LastDir - (uintptr_t)Current;
        char* LastPath = (char*)malloc(Len + 1);
        memcpy(LastPath, Current, Len);
        LastPath[Len] = '\0';
        return LastPath;
    }
}

void OpenFolder(kui_Context* Ctx, char* Path){
    DIR* Tmp = opendir(Path);
    size_t Len = strlen(Path);
    if(Tmp){
        free(Ctx->opaque);
        closedir(Directory);
        Directory = Tmp;
        char* TmpPath = (char*)malloc(Len + 1);
        memcpy(TmpPath, Path, Len);
        TmpPath[Len] = '\0';
        Ctx->opaque = (void*)TmpPath;
        memcpy(PathBarBuffer, TmpPath, strlen(TmpPath) + 1);
        UpdateEntries();
    }
}

int AddField(kui_Context* Ctx, char* Text, int IconId){
    int res = 0;
    kui_Id id = kui_get_id(Ctx, Text, strlen(Text));
    kui_Rect r = kui_layout_next(Ctx);
    kui_update_control(Ctx, id, r, 0);
    /* handle click */
    if (Ctx->mouse_pressed == KUI_MOUSE_LEFT && Ctx->focus == id) {
        res |= KUI_RES_SUBMIT;
    }
    /* draw */
    kui_draw_control_frame(Ctx, id, r, KUI_COLOR_BUTTONDARK, KUI_OPT_NOBORDER);
    kui_Rect RIcon = r;
    RIcon.w = RIcon.h;
    kui_Rect RText = r;
    RText.x += RIcon.w;
    RText.w -= RIcon.w;
    kui_draw_icon(Ctx, IconId, RIcon, Ctx->style->colors[KUI_COLOR_TEXT]);
    kui_draw_control_text(Ctx, Text, RText, KUI_COLOR_TEXT, 0);
    return res;
}

void DrawHeader(kui_Context* Ctx){
    kui_Rect R = kui_get_current_container(Ctx)->body;
    kui_layout_row(Ctx, 5, (int[]){25, 25, R.w - 126, 25, 25}, 27);
    if(kui_button_ex(Ctx, NULL, 1462, 0)){
        char* TmpPath = LastPath((char*)Ctx->opaque);
        if(TmpPath){
            OpenFolder(Ctx, TmpPath);
            free(TmpPath);
        }
    }
    if(kui_button_ex(Ctx, NULL, 1463, 0)){

    }

    if(kui_textbox(Ctx, PathBarBuffer, sizeof(PathBarBuffer)) & KUI_RES_SUBMIT){
        size_t Len = strlen(PathBarBuffer);
        if(Len){
            kui_set_focus(Ctx, Ctx->last_id);
            OpenFolder(Ctx, PathBarBuffer);
        }
    }
    
    if(kui_button_ex(Ctx, NULL, 873, 0)){

    }
    if(kui_button_ex(Ctx, NULL, 874, 0)){

    }
}

void OpenSystemBookmarks(kui_Context* Ctx, char* Name){
    char* TmpPath = (char*)malloc(strlen(USER_FOLDER) + strlen(Name) + 1);
    TmpPath[0] = '\0';
    strcat(TmpPath, USER_FOLDER);
    strcat(TmpPath, Name);
    if(TmpPath){
        OpenFolder(Ctx, TmpPath);
        free(TmpPath);
    }
}

void DrawBookmarks(kui_Context* Ctx){
    kui_layout_row(Ctx, 2, (int[]){150, -1}, -1);
    kui_begin_panel(Ctx, "Bookmarks");

    kui_layout_row(Ctx, 1, (int[]){-1}, 25);
    if(AddField(Ctx, "Home", 1250)){
        OpenSystemBookmarks(Ctx, "Home");
    }

    kui_layout_row(Ctx, 1, (int[]){-1}, 25);
    if(AddField(Ctx, "Desktop", 1240)){
        OpenSystemBookmarks(Ctx, "Desktop");
    }

    kui_layout_row(Ctx, 1, (int[]){-1}, 25);
    if(AddField(Ctx, "Documents", 1545)){
        OpenSystemBookmarks(Ctx, "Documents");
    }

    kui_layout_row(Ctx, 1, (int[]){-1}, 25);
    if(AddField(Ctx, "Downloads", 1254)){
        OpenSystemBookmarks(Ctx, "Downloads");
    }

    kui_layout_row(Ctx, 1, (int[]){-1}, 25);
    if(AddField(Ctx, "Music", 1231)){
        OpenSystemBookmarks(Ctx, "Music");
    }

    kui_layout_row(Ctx, 1, (int[]){-1}, 25);
    if(AddField(Ctx, "Pictures", 1289)){
        OpenSystemBookmarks(Ctx, "Pictures");
    }

    kui_layout_row(Ctx, 1, (int[]){-1}, 25);
    if(AddField(Ctx, "Videos", 1238)){
        OpenSystemBookmarks(Ctx, "Videos");
    }

    kui_layout_row(Ctx, 1, (int[]){-1}, 25);
    if(AddField(Ctx, "Trash", 4945)){
        OpenSystemBookmarks(Ctx, "Trash");
    }

    kui_end_panel(Ctx);
}

void DrawFiles(kui_Context* Ctx){
    kui_begin_panel(Ctx, "Files");
    for(uint64_t i = 0; i < EntriesCount; i++){
        struct dirent* Entry = Entries[i];
        kui_layout_row(Ctx, 1, (int[]){-1}, 25);
        if(AddField(Ctx, Entry->d_name, (Entry->d_type == DT_REG) ? 2796 : 2851)){
            if(Entry->d_type == DT_DIR){
                char* TmpPath = NextPath((char*)Ctx->opaque, Entry->d_name);
                OpenFolder(Ctx, TmpPath);
                free(TmpPath);
            }else{
                char* Extension = strrchr(Entry->d_name, '.');
                if(Extension){
                    if(strlen(Extension) > 1){
                        Extension++;
                        // TODO
                    }
                }else{
                    size_t Length = strlen(PathBarBuffer);
                    char* Path = (char*)malloc(Length + sizeof('/') + strlen(Entry->d_name) + 1);
                    Path[0] = '\0';
                    strcat(Path, PathBarBuffer);
                    if(PathBarBuffer[Length - 1] != '/'){
                        strcat(Path, "/");
                    }
                    strcat(Path, Entry->d_name);

                    char* Argv[] = {Path, NULL};
                    char* Env[] = {NULL};

                    kot_launch(Path, Argv, Env);
                    free(Path);
                }
            }
        }
    }
    kui_end_panel(Ctx);
}

void WindowRenderer(kui_Context* Ctx){
    kui_Container* Cnt;

    kui_begin(Ctx);

    if(kui_begin_window(Ctx, "File explorer", kui_rect(50, 50, 900, 400))){
        Cnt = kui_get_current_container(Ctx);

        DrawHeader(Ctx);

        DrawBookmarks(Ctx);

        DrawFiles(Ctx);

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
    UpdateEntries();
    kui_init(WindowRenderer, (void*)Root);
    kot_Sys_Close(KSUCCESS); /* Don't close the process */
}