#include <cstddef>
#if __WORDSIZE == 64
typedef unsigned long int           uint64_t;
#else
typedef unsigned long long int      uint64_t;
#endif

struct Framebuffer{
	void* BaseAddress;
	void* BaseAddressBackground;
    size_t FrameBufferSize;
	unsigned int Width;
	unsigned int Height;
	unsigned int PixelsPerScanLine;
}__attribute__((packed));

uint64_t DoSyscall(uint64_t syscall, uint64_t arg0, uint64_t arg1, uint64_t arg2, uint64_t arg3, uint64_t arg4, uint64_t arg5){
    asm("movq %0, %%rdi" :: "r" (arg0));
    asm("movq %0, %%rsi" :: "r" (arg1));
    asm("movq %0, %%rdx" :: "r" (arg2));
    asm("movq %0, %%r10" :: "r" (arg3));
    asm("movq %0, %%r8" :: "r" (arg4));
    asm("movq %0, %%r9" :: "r" (arg5));
    asm("movq %0, %%rax" :: "r" (syscall));
    asm("int $0x80");
}

Framebuffer framebuffer;

void Putpixel(int x, int y, int r, int g, int b) {
    unsigned char* screen = (unsigned char*)framebuffer.BaseAddress;
    int where = (x + (y * framebuffer.Width)) * 4;
    screen[where] = b;          // BLUE
    screen[where + 1] = g;      // GREEN
    screen[where + 2] = r;      // RED
}

void Rectangle(int w, int h, int x, int y, int r, int g, int b) {
    for (int i = 0; i < h; i++) {
        for (int j = 0; j < w; j++) {
            Putpixel(j + x, i + y, r, g, b);
        }
    }
}

void main(int test){    
    uint64_t GetValue = DoSyscall(0, 0, (uint64_t)&framebuffer, 0, 0, 0, 0);
    Rectangle(50, 50, 0, 0, 0xff, 0x50, 0x10);
    DoSyscall(1, (uint64_t)framebuffer.BaseAddress, 0, 0, 0, 0, 0);

    while(true);
}