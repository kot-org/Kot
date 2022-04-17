#include <kot/sys/sys.h>
#include "main.h"

void Putpixel(stivale2_struct_tag_framebuffer* framebuffer, int x, int y, int r, int g, int b) {
    unsigned char* screen = (unsigned char*)framebuffer->framebuffer_addr;
    int where = (x + (y * framebuffer->framebuffer_width)) * 4;
    screen[where] = b;          // BLUE
    screen[where + 1] = g;      // GREEN
    screen[where + 2] = r;      // RED
}

void Putpixel(stivale2_struct_tag_framebuffer* framebuffer, int x, int y, uint64_t pixel) {
    unsigned char* screen = (unsigned char*)framebuffer->framebuffer_addr;
    int where = (x + (y * framebuffer->framebuffer_width)) * 4;
    screen[where] = pixel;
}

void Rectangle(stivale2_struct_tag_framebuffer* framebuffer, int w, int h, int x, int y, int r, int g, int b) {
    for (int i = 0; i < h; i++) {
        for (int j = 0; j < w; j++) {
            Putpixel(framebuffer, j + x, i + y, r, g, b);
        }
    }
}
 
int main(KernelInfo* kernelInfo){
    int i = 1;
    Syscall_0(0);
    SYS_Pause(_main_thread);
    while(true){
        Rectangle(kernelInfo->framebuffer, 500, 500, i * 500, 0, 0xee, 0xf1, 0xf);
        i++;
    }
       
    while(true);
}
