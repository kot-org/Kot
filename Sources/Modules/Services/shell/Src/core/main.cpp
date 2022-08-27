#include "main.h"

#include "psf1.h"

extern "C" int main() {

    process_t self = Sys_GetProcess();

    uint32_t wid = orb::Create(300, 300, 10, 10);
    uintptr_t fb = orb::GetFramebuffer(wid);

    srv_system_callback_t* file = Srv_System_ReadFileInitrd("zap-light16.psf", true);
    psf1_font* zap_light16 = psf1_parse(file->Data);

    psf1_putchar(zap_light16, fb, 300 * 4, 'h', 8*0, 0, 0xffffffff);
    psf1_putchar(zap_light16, fb, 300 * 4, 'e', 8*1, 0, 0xffffffff);
    psf1_putchar(zap_light16, fb, 300 * 4, 'l', 8*2, 0, 0xffffffff);
    psf1_putchar(zap_light16, fb, 300 * 4, 'l', 8*3, 0, 0xffffffff);
    psf1_putchar(zap_light16, fb, 300 * 4, 'o', 8*4, 0, 0xffffffff);
    // ' '
    psf1_putchar(zap_light16, fb, 300 * 4, 'w', 8*6, 0, 0xffffffff);
    psf1_putchar(zap_light16, fb, 300 * 4, 'o', 8*7, 0, 0xffffffff);
    psf1_putchar(zap_light16, fb, 300 * 4, 'r', 8*8, 0, 0xffffffff);
    psf1_putchar(zap_light16, fb, 300 * 4, 'l', 8*9, 0, 0xffffffff);
    psf1_putchar(zap_light16, fb, 300 * 4, 'd', 8*10, 0, 0xffffffff);

    return KSUCCESS;

}