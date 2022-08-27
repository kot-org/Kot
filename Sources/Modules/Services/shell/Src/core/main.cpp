#include "main.h"

extern "C" int main() {

    process_t self = Sys_GetProcess();

    uint32_t wid = orb::Create(400, 300, 10, 10);
    uintptr_t fb = orb::GetFramebuffer(wid);
    
    Context* ctx = new Context(fb, 400, 300);

    ctx->fillRect(0, 0, 100, 100, 0xffffff);

    return KSUCCESS;

}