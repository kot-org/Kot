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

void readBMP(struct stivale2_struct_tag_framebuffer* framebuffer, void* buffer, size_t size, uint64_t height, int Lx, int Ly, bool AlignMiddle){
    uint8_t* ReadBuffer = (uint8_t*)buffer;

    //read the header info
    int i = 54;
    int y = 0;
    unsigned char info[54];
    while(i > 0)
    {
        uint8_t g = *ReadBuffer;
        info[y] = g;
        ReadBuffer++;
        y++;
        i--;
    }
    int dataOffset = *(int*)&info[10]; 
    int src_width = *(int*)&info[18];
    int src_height = *(int*)&info[22];
    int width = src_width * height / src_height;
    int bitCount = (*(short*)&info[28]) / 8;
    //location   
    if(AlignMiddle){ 
        Lx = (framebuffer->framebuffer_width - width) / 2;
        Ly = (framebuffer->framebuffer_height - height) / 2;
    }
    int location = (Lx + Ly * framebuffer->framebuffer_width) * 4;

    unsigned char *screen = (unsigned char*)framebuffer->framebuffer_addr;

    int pixelSize = 2;
    ReadBuffer = (uint8_t*)buffer;
    ReadBuffer += dataOffset;
    //resize image to fill the screen
    for (int i = height; 0 < i; i--) { 
        for (int j = 0; j < width; j++) {
            int where = (j + (i * framebuffer->framebuffer_width)) * 4 + location;          
            for(int c = pixelSize; 0 <= c; c--){
                uint8_t g = ReadBuffer[((j * src_width) / width + (((height - i) * src_height) / height) * src_width) * bitCount + c];
                //verify if the picture is in the screen
                if(where < framebuffer->framebuffer_pitch * framebuffer->framebuffer_height){
                    //so you can draw the pixel 
                    switch(c){
                        case 0: 
                            screen[where] = g; 
                            break;
                        case 1:
                            screen[where + 1] = g; 
                            break;
                        case 2:
                            screen[where + 2] = g;
                            break;
                        default: 
                            break;
                    }            
                            
                }                                                       
            }                        
        }
    }
}

int main(struct KernelInfo* kernelInfo){
    kthread_t self;
    SYS_GetThreadKey(&self);
    InitializeHeap();

    ramfs::Parse(kernelInfo->ramfs->ramfsBase, kernelInfo->ramfs->Size);
    
    ramfs::File* Wallpaper = ramfs::Find("Wallpaper.bmp");
    
    if(Wallpaper != NULL){
        void* BufferWallpaper = malloc(Wallpaper->size);
        ramfs::Read(Wallpaper, BufferWallpaper);
        readBMP(kernelInfo->framebuffer, BufferWallpaper, Wallpaper->size, kernelInfo->framebuffer->framebuffer_height, 0, 0, false);
    }
    SYS_Pause(self);
}