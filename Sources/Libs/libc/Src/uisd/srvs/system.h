#ifndef _SRV_SYSTEM_H
#define _SRV_SYSTEM_H 1

#include <kot/types.h>
#include <kot/uisd.h>
#include <kot/sys.h>

#if defined(__cplusplus)
extern "C" {
#endif

typedef KResult (*CallbackHandler)(KResult Statu, struct srv_system_callback_t* Callback, uint64_t GP0, uint64_t GP1, uint64_t GP2, uint64_t GP3);

typedef struct {
    uint64_t address;
    uint64_t width;
    uint64_t height;
    uint64_t pitch;
    uint64_t bpp;
} srv_system_framebuffer_t;

struct srv_system_callback_t{
    thread_t Self;
    uintptr_t Data;
    bool IsAwait;
    KResult Statu;
    CallbackHandler Handler;
};

void Srv_System_Initialize();

void Srv_System_Callback(KResult Statu, struct srv_system_callback_t* Callback, uint64_t GP0, uint64_t GP1, uint64_t GP2, uint64_t GP3);

struct srv_system_callback_t* Srv_System_GetFrameBufer(srv_system_framebuffer_t* framebuffer, bool IsAwait);

#if defined(__cplusplus)
}
#endif

#endif