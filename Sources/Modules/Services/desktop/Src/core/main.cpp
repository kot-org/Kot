#include <core/main.h>

void createDesktop() {

}

extern "C" int main(int argc, char* argv[]){
    Printlog("[DESKTOP] Initializing...");

    // load desktopUserSettings.json
    window_t* Window = CreateWindow(NULL, Window_Type_Background);
    ResizeWindow(Window, Window_Max_Size, Window_Max_Size);



    Printlog("[DESKTOP] Initialized");
    return KSUCCESS;
}