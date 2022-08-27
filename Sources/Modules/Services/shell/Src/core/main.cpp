#include "main.h"

extern "C" int main() {

    process_t self = Sys_GetProcess();

    uint32_t wid = orb::Create(400, 300, 10, 10);
    uintptr_t fb = orb::GetFramebuffer(wid);
    uint64_t pitch = 400 * 300 * 4;
    
    for (uint8_t x = 0; x < 100; x++) {
        for (uint8_t y = 0; y < 100; y++) {
            uint64_t index = x * 4 + y * pitch;
            *(uint32_t*)((uint64_t) fb + index) = 0xffffff;
        }
    }

}