#pragma once

#include <core/main.h>

#include <kot-ui/core.h>

struct shell_t{
    kui_Context* Ctx;

    kot_process_t Target;

    kot_vector_t* ReadRequest;

    char* OutputBuffer;
    size64_t OutputBufferSize;
    char* OuputBufferLastShow;

    char* InputBuffer;
    size64_t InputBufferSize;

    kot_framebuffer_t TextFramebuffer;

    kfont_t ShellFont;

    kot_event_t Event;

    uint64_t LineNumberShow;
    uint64_t LineNumberMax;
    uint64_t Width;
    uint64_t Height;

    uint64_t Lock;
};

struct read_request_shell_t{
    kot_thread_t Callback;
    uint64_t CallbackArg;

    size64_t SizeRequest;
    size64_t SizeGet;
    char* Buffer;
};

struct shell_t* NewShell(kot_process_t Target);

void ShellPrintWU(shell_t* Shell, void* Buffer, size64_t Size);
void ShellPrint(shell_t* Shell, void* Buffer, size64_t Size);

KResult ShellSendRequest(shell_t* Shell, read_request_shell_t* Request);
KResult ShellCreateRequest(shell_t* Shell, kot_thread_t Callback, uint64_t CallbackArg, size64_t SizeRequest);

void ShellEventEntry(uint64_t EventType, uint64_t GP0, uint64_t GP1, uint64_t GP2, uint64_t GP3, uint64_t GP4);