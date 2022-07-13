#include <core/main.h>

int main(int argc, char* argv[], struct framebuffer_t* Framebuffer){
    Printlog("[WINDOWS MANAGER] Initialization ...");
    kprocess_t self;
    SYS_GetProcessKey(&self);
    uint64_t virtualAddress = KotSpecificData.FreeMemorySpace - (Framebuffer->framebuffer_pitch * Framebuffer->framebuffer_height);
    size_t frameBufferSize = Framebuffer->framebuffer_pitch * Framebuffer->framebuffer_height;
    SYS_Map(self, &virtualAddress, true, &Framebuffer->framebuffer_addr, &frameBufferSize, false);
    memset(virtualAddress, 0xff, frameBufferSize);
    return KSUCCESS;
}