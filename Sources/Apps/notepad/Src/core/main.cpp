#include <core/main.h>

void DrawHeader(kui_Context* Ctx){

}

void DrawTextarea(kui_Context* Ctx){
    char text[1024];

    kui_layout_row(Ctx, 1, (int[]){-1}, 50);
    if(kui_textbox(Ctx, text, sizeof(text))) {

    }
}

void WindowRenderer(kui_Context* Ctx){
    kui_Container* Cnt;

    kui_begin(Ctx);

    if(kui_begin_window(Ctx, "Notepad", kui_rect(50, 50, 900, 400))){
        Cnt = kui_get_current_container(Ctx);

        DrawHeader(Ctx);

        DrawTextarea(Ctx);

        kui_end_window(Ctx);
    }
    kui_end(Ctx);
    kui_r_present(Cnt);
}

int main(int argc, char* argv[]){
    kui_init(WindowRenderer, NULL);
    kot_Sys_Close(KSUCCESS); /* Don't close the process */
}