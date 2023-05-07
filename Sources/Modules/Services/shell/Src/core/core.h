#pragma once

#include <core/main.h>
#include <kot-graphics/utils.h>

struct shell_t{
    framebuffer_t* Framebuffer;
    framebuffer_t* Backbuffer;
    kfont_t* Font;

    window_t* Wid;

    process_t Target;

    uint64_t HeightUsed;

    event_t ShellEvent;
    thread_t ShellEventThread;

    uint64_t PressedCache;

    vector_t* ReadRequest;
};

struct read_request_shell_t{
    thread_t Callback;
    uint64_t CallbackArg;

    size64_t SizeRequest;
    size64_t SizeGet;
    char* Buffer;
};

// Taken from ssfn.h
struct ssfn_buf_t{
    uint8_t *ptr;                     /* pointer to the buffer */
    int w;                            /* Width (positive: ARGB, negative: ABGR pixels) */
    int h;                            /* Height */
    uint16_t p;                       /* Pitch, bytes per line */
    int x;                            /* cursor x */
    int y;                            /* cursor y */
    uint32_t fg;                      /* foreground color */
    uint32_t bg;                      /* background color */
};

struct shell_t* NewShell(process_t Target);

void ShellPrint(shell_t* Shell, uintptr_t Buffer, size64_t Size);

KResult ShellSendRequest(shell_t* Shell, read_request_shell_t* Request);
KResult ShellCreateRequest(shell_t* Shell, thread_t Callback, uint64_t CallbackArg, size64_t SizeRequest);

void ShellEventEntry(uint64_t EventType, uint64_t GP0, uint64_t GP1, uint64_t GP2, uint64_t GP3, uint64_t GP4);