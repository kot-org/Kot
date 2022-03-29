#include <kot/sys/sys.h>
#include "main.h"

void Putpixel(Framebuffer* framebuffer, int x, int y, int r, int g, int b) {
    unsigned char* screen = (unsigned char*)framebuffer->BaseAddress;
    int where = (x + (y * framebuffer->Width)) * 4;
    screen[where] = b;          // BLUE
    screen[where + 1] = g;      // GREEN
    screen[where + 2] = r;      // RED
}

void Putpixel(Framebuffer* framebuffer, int x, int y, uint64_t pixel) {
    unsigned char* screen = (unsigned char*)framebuffer->BaseAddress;
    int where = (x + (y * framebuffer->Width)) * 4;
    screen[where] = pixel;
}

void Rectangle(Framebuffer* framebuffer, int w, int h, int x, int y, int r, int g, int b) {
    for (int i = 0; i < h; i++) {
        for (int j = 0; j < w; j++) {
            Putpixel(framebuffer, j + x, i + y, r, g, b);
        }
    }
}
 
int main(KernelInfo* kernelInfo){
    uint64_t VirtualStart = 0x100000;
    uint64_t DoubleVirtualStart = VirtualStart + kernelInfo->framebuffer->FrameBufferSize;
    uint64_t itierator = 0;

    for(int i = 0; i < kernelInfo->framebuffer->FrameBufferSize; i += 0x1000){
        DoSyscall(0x8, DoubleVirtualStart + itierator, false, 0, 0, 0, 0);  
        DoSyscall(0x8, VirtualStart, true, (uint64_t)kernelInfo->framebuffer->BaseAddress + i, 0, 0, 0);  
        VirtualStart += 0x1000;
        itierator += 0x1000;
    }

    kernelInfo->framebuffer->BaseAddress = (void*)0x100000;

    int i = 1;
    while(true){
        Rectangle(kernelInfo->framebuffer, 500, 500, i * 500, 0, 0xee, 0xf1, 0xf);
        i++;
    }
       
    while(true);
}
