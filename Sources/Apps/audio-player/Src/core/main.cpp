#include <core/main.h>


DIR* Directory;
dirent* Entries[256];
uint64_t EntriesCount;

static char* PathBuffer;

#define ROOT_PATH "/d1:/user/root/Music"
#define ROOT_PATH_LEN strlen(ROOT_PATH)

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
        closedir(Directory);
        Directory = Tmp;
        memcpy(PathBuffer, Path, Len);
        PathBuffer[Len] = '\0';
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

void DrawPlaylist(kui_Context* Ctx, AudioHandler* Handler){
    kui_begin_panel(Ctx, "Playlist");
    kui_layout_row(Ctx, 1, (int[]){-1}, 25);
    if(AddField(Ctx, "..", 2851)){
        char* TmpPath = LastPath(PathBuffer);
        if(TmpPath){
            OpenFolder(Ctx, TmpPath);
            free(TmpPath);
        }
    }
    for(uint64_t i = 0; i < EntriesCount; i++){
        struct dirent* Entry = Entries[i];
        kui_layout_row(Ctx, 1, (int[]){-1}, 25);
        bool IsMusicFile = false;
        char* Extension = strrchr(Entry->d_name, '.');
        int Icon = (Entry->d_type == DT_REG) ? 2796 : 2851;
        if(Extension){
            if(strlen(Extension) > 1){
                Extension++;
                if(strcmp(Extension, "mp3") == 0){
                    IsMusicFile = true;
                    Icon = 2811;
                }
            }
        }

        if(AddField(Ctx, Entry->d_name, Icon)){
            if(Entry->d_type == DT_DIR){
                char* TmpPath = NextPath(PathBuffer, Entry->d_name);
                kot_Printlog(TmpPath);
                OpenFolder(Ctx, TmpPath);
                free(TmpPath);
            }else{
                if(IsMusicFile){
                    size_t Length = strlen(PathBuffer);
                    char* FilePath = (char*)malloc(Length + strlen((char*)Entry->d_name) + sizeof('/') + 1);
                    FilePath[0] = '\0';
                    strcat(FilePath, (char*)PathBuffer);
                    if(PathBuffer[Length - 1] != '/'){
                        strcat(FilePath, "/");
                    }
                    strcat(FilePath, Entry->d_name);
                    Handler->SetPlay(false);
                    Handler->LoadTrack(FilePath);
                    Handler->SetPlay(true);
                    free(FilePath);
                }
            }
        }
    }
    kui_end_panel(Ctx);
}

void DrawTrack(kui_Context* Ctx, AudioHandler* Handler){
    kui_Rect R = kui_get_current_container(Ctx)->body;
    kui_layout_row(Ctx, 2, (int[]){150, -1}, R.h - 65);
    kui_begin_panel(Ctx, "Track");
    kui_layout_row(Ctx, 1, (int[]){ -1}, 20);
    char TextBuffer[1024];
    sprintf((char*)&TextBuffer, "Title : %s", Handler->GetTrackInfo()->Metadata.Title);
    kui_text(Ctx, TextBuffer);
    kui_layout_row(Ctx, 1, (int[]){ -1}, 20);
    sprintf((char*)&TextBuffer, "Duration : %02" PRId64 ":%02" PRId64 ":%02" PRId64, Handler->GetTrackInfo()->Hours, Handler->GetTrackInfo()->Mins, Handler->GetTrackInfo()->Secs);
    kui_text(Ctx, TextBuffer);
    kui_layout_row(Ctx, 1, (int[]){ -1}, 20);
    sprintf((char*)&TextBuffer, "Artist : %s", Handler->GetTrackInfo()->Metadata.Artist);
    kui_text(Ctx, TextBuffer);
    kui_layout_row(Ctx, 1, (int[]){ -1}, 20);
    sprintf((char*)&TextBuffer, "Album : %s", Handler->GetTrackInfo()->Metadata.Album);
    kui_text(Ctx, TextBuffer);
    kui_end_panel(Ctx);
}

int Progressbar(kui_Context* Ctx, float* Value) {
    float Tmp;
    static bool IsFocus = false;

    kui_push_id(Ctx, &Value, sizeof(Value));

    Tmp = *Value;

    int Res = kui_slider_ex(Ctx, &Tmp, 0, 1000, 0, "", KUI_OPT_ALIGNCENTER);

    *Value = Tmp;

    kui_pop_id(Ctx);

    return Res;
}

void DrawControls(kui_Context* Ctx, AudioHandler* Handler){
    kui_Rect R = kui_get_current_container(Ctx)->body;
    kui_layout_set_next(Ctx, kui_rect(R.x, R.y + R.h - 55, R.w, 55), 0);
    kui_begin_panel(Ctx, "Controls");
    R = kui_get_current_container(Ctx)->body;

    kui_layout_set_next(Ctx, kui_rect(R.x, R.y, R.w, 15), 0);
    float Progression;
    if(Handler->GetTrackInfo()->Duration){
        Progression = (Handler->GetCurrentTimestamp() * (float)1000) / Handler->GetTrackInfo()->Duration;
    }
    if(Progressbar(Ctx, &Progression) & KUI_RES_CHANGE){
        float NewTimestamp = (Progression * Handler->GetTrackInfo()->Duration) / 1000;
        Handler->SetCurrentTimestamp(NewTimestamp);
    }

    R.h = 25;
    R.y += 25;

    kui_layout_set_next(Ctx, kui_rect(R.x + 10, R.y, 150, 25), 0);
    char TextBuffer[1024];
    int64_t CurrentSeconds = Handler->GetCurrentTimestamp();
    int64_t CurrentMinutes = CurrentSeconds / 60;
    CurrentSeconds %= 60;
    int64_t CurrentHours = CurrentMinutes / 60;
    CurrentMinutes %= 60;

    sprintf((char*)&TextBuffer, "%02d:%02d:%02d/%02d:%02d:%02d", CurrentHours, CurrentMinutes, CurrentSeconds, Handler->GetTrackInfo()->Hours, Handler->GetTrackInfo()->Mins, Handler->GetTrackInfo()->Secs);
    kui_text(Ctx, TextBuffer);

    R.x = (R.w - 3 * (R.h + 10)) / 2;
    kui_layout_set_next(Ctx, kui_rect(R.x, R.y, R.h, R.h), 0);
    if(kui_button_ex(Ctx, NULL, 1300, 0)){

    }
    R.x += R.h + 10;
    kui_layout_set_next(Ctx, kui_rect(R.x, R.y, R.h, R.h), 0);
    if(kui_button_ex(Ctx, NULL, Handler->GetPlay() ? 1302 : 1301, 0)){
        Handler->SetPlay(!Handler->GetPlay());
    }
    R.x += R.h + 10;
    kui_layout_set_next(Ctx, kui_rect(R.x, R.y, R.h, R.h), 0);
    if(kui_button_ex(Ctx, NULL, 1304, 0)){

    }
    kui_end_panel(Ctx);
}

void WindowRenderer(kui_Context* Ctx){
    AudioHandler* Handler = (AudioHandler*)Ctx->opaque;

    kui_Container* Cnt;

    kui_begin(Ctx);

    if(kui_begin_window(Ctx, "Audio player", kui_rect(50, 50, 900, 400))){
        Cnt = kui_get_current_container(Ctx);

        DrawTrack(Ctx, Handler);

        DrawPlaylist(Ctx, Handler);

        DrawControls(Ctx, Handler);


        kui_end_window(Ctx);
    }
    kui_end(Ctx);
    kui_r_present(Cnt); 
}

int main(int argc, char* argv[]){
    AudioHandler* Handler = new AudioHandler();

    Handler->SetStream(new Audio::Stream(0));

    char* Root = (char*)malloc(ROOT_PATH_LEN + 1);
    memcpy(Root, ROOT_PATH, ROOT_PATH_LEN + 1);
    PathBuffer = Root;
    Directory = opendir(Root);
    UpdateEntries();

    kui_Context* Ctx = kui_init(WindowRenderer, (void*)Handler);
    Handler->OnNewFrameEvent = Ctx->containers[0].window_parent->window_event;
    Handler->OnNewFrameEventArgs.arg[0] = Window_Event_Update;
    
    kot_Sys_Close(KSUCCESS); /* Don't close the process */
}