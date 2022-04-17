#include <kot/sys/sys.h>
#include <main/main.h>

void Putpixel(stivale2_struct_tag_framebuffer* framebuffer, int x, int y, int r, int g, int b) {
    unsigned char* screen = (unsigned char*)framebuffer->framebuffer_addr;
    int where = (x + (y * framebuffer->framebuffer_width)) * framebuffer->framebuffer_bpp;
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
    Rectangle(kernelInfo->framebuffer, 500, 500, 0, 0, 231, 76, 60);
    while(true);
}
