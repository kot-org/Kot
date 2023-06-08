#pragma once

#include <core/main.h>

#define ORB_Srv_Version 0x1

class orbc;
class windowc;
class monitorc;
class desktopc;
class renderc;
class hidc;

typedef KResult (*window_dispatch_t)(kot_thread_t Callback, uint64_t CallbackArg, windowc* window, uint64_t GP0, uint64_t GP1, uint64_t GP2);

KResult InitialiseServer(orbc* Orb);

KResult CreateWindowSrv(kot_thread_t Callback, uint64_t CallbackArg, kot_process_t Target, kot_thread_t Event, uint64_t WindowType);

KResult WindowGraphicsHandler(kot_thread_t Callback, uint64_t CallbackArg, uint64_t GP0, uint64_t GP1, uint64_t GP2, uint64_t GP3);

KResult WindowClose(kot_thread_t Callback, uint64_t CallbackArg, windowc* Window, uint64_t GP0, uint64_t GP1, uint64_t GP2);
KResult WindowResize(kot_thread_t Callback, uint64_t CallbackArg, windowc* Window, uint64_t GP0, uint64_t GP1, uint64_t GP2);
KResult WindowChangePostion(kot_thread_t Callback, uint64_t CallbackArg, windowc* Window, uint64_t GP0, uint64_t GP1, uint64_t GP2);
KResult WindowChangeVisibility(kot_thread_t Callback, uint64_t CallbackArg, windowc* Window, uint64_t GP0, uint64_t GP1, uint64_t GP2);