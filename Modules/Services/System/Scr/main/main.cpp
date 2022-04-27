#include <kot/sys.h>
#include <kot/heap.h>
#include <kot/memory.h>
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
    kthread_t self;
    SYS_GetThreadKey(&self);
    InitializeHeap();

    memset(malloc(0x1000), 0xff, 0x1000);
    memset(malloc(0x4000), 0xff, 0x4000);

    for(int i = 0; i < 100; i++){
        Rectangle(kernelInfo->framebuffer, 500, 1000, i * 10, i * 20, 255, i * 10, i * 25);
    }
    SYS_Pause(self);
}
