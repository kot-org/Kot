#include <core/main.h>

void DrawHeader(kui_Context* Ctx){

}

void DrawControls(kui_Context* Ctx){

}

void WindowRenderer(kui_Context* Ctx){
    kui_Container* Cnt;

    kui_begin(Ctx);

    if(kui_begin_window(Ctx, "Audio player", kui_rect(50, 50, 900, 400))){
        Cnt = kui_get_current_container(Ctx);

        DrawHeader(Ctx);

        DrawControls(Ctx);

        kui_end_window(Ctx);
    }
    kui_end(Ctx);
    kui_r_present(Cnt);
}

int main(int argc, char* argv[]){
    kui_init(WindowRenderer, NULL);
    return KSUCCESS;
}