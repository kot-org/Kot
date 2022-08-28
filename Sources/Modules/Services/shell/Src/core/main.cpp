#include "main.h"

#include "psf1.h"

extern "C" int main() {
    Printlog("[Shell]Loading");
    process_t self = Sys_GetProcess();

    uint32_t wid = orb::Create(300, 300, 10, 10);
    uintptr_t fb = orb::GetFramebuffer(wid);

    srv_system_fileheader_t* File = (srv_system_fileheader_t*)malloc(sizeof(srv_system_fileheader_t));
    srv_system_callback_t* Data = Srv_System_ReadFileInitrd("default-font.sfn", File, true);
    
    Printlog("[Shell]Ok");
    kfont_t* font = LoadFont(File->Data, font_family_serif, NULL, NULL, 10);
    
    font_fb_t buffer;
    buffer.address = fb;
    buffer.width = 300;
    buffer.height = 300;
    buffer.pitch = 300 * 4;
    PrintFont(font, "Hello", &buffer, 0, 0, 0xffffff);
    FreeFont(font);
    Printlog("[Shell]End");

    return KSUCCESS;

}