#pragma once

#include <stdio.h>
#include <assert.h>
#include <termios.h>

#include <core/main.h>
#include <port/term.h>


typedef struct{
    struct term_t Term;

    kot_vector_t* ReadRequest;

    kot_event_t Event;

    int TerminalID;

    char* InputBuffer;
    size64_t InputBufferSize;
    uint64_t InputCursorPos;

    struct termios Terminos;
    struct winsize Winsize;

    pid_t ForegroundPID;
}kot_term_t;


typedef struct{
    kot_thread_t Callback;
    uint64_t CallbackArg;

    size64_t SizeRequest;
    char* Buffer;

    kot_process_t TargetDataProc;
}read_request_shell_t;

kot_term_t* CreateTerminal(kot_framebuffer_t* Fb, char* FontPath, char* ImagePath);

void PutCharTerminal(kot_term_t* Handler, char Char);
void PrintTerminal(kot_term_t* Handler, const char* Text);
void WriteTerminal(kot_term_t* Handler, const char* Text, size_t Size);

void BackspaceTerminal(kot_term_t* Handler);

KResult SendRequestTerminal(kot_term_t* Handler, read_request_shell_t* Request);
KResult CreateRequestTerminal(kot_term_t* Handler, kot_thread_t Callback, uint64_t CallbackArg, size64_t SizeRequest, kot_process_t TargetDataProc);

void PressKeyTerminal(kot_term_t* Handler, uint64_t Key);