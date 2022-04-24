#include <kot/sys.h>
#include <kot/heap.h>
#include <main/main.h>

void Putpixel(struct stivale2_struct_tag_framebuffer* framebuffer, int x, int y, int r, int g, int b) {
    unsigned char* screen = (unsigned char*)framebuffer->framebuffer_addr;
    int where = (x + (y * framebuffer->framebuffer_width)) * (framebuffer->framebuffer_bpp / 8);
    screen[where] = b;          // BLUE
    screen[where + 1] = g;      // GREEN
    screen[where + 2] = r;      // RED
}

void Rectangle(struct stivale2_struct_tag_framebuffer* framebuffer, int w, int h, int x, int y, int r, int g, int b) {
    for (int i = 0; i < h; i++) {
        for (int j = 0; j < w; j++) {
            Putpixel(framebuffer, j + x, i + y, r, g, b);
        }
    }
}
 
int main(struct KernelInfo* kernelInfo){
    Rectangle(kernelInfo->framebuffer, 500, 500, 0, 0, 231, 76, 60);
    SYS_Map(0,0,0,0,0,0);
    Rectangle(kernelInfo->framebuffer, 500, 500, 0, 0, 0, 76, 60);
    SYS_Map(0,0,0,0,0,0);
    SYS_Exit(_process, 0);
    Rectangle(kernelInfo->framebuffer, 500, 500, 500, 500, 0, 76, 60);
    malloc(0x1000);
    Rectangle(kernelInfo->framebuffer, 500, 500, 500, 0, 0, 255, 60);
    InitializeHeap(_process);
    Rectangle(kernelInfo->framebuffer, 500, 500, 1000, 0, 0, 0, 255);
    while(true);
}
