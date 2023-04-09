#pragma once

#include <core/main.h>

#define ORB_Srv_Version 0x1

class orbc;
class windowc;
class monitorc;
class desktopc;
class renderc;
class mousec;

typedef KResult (*window_dispatch_t)(thread_t Callback, uint64_t CallbackArg, windowc* window, uint64_t GP0, uint64_t GP1, uint64_t GP2);

KResult InitialiseServer(orbc* Orb);

KResult CreateWindow(thread_t Callback, uint64_t CallbackArg, process_t Target, thread_t Event, uint64_t WindowType);

KResult WindowGraphicsHandler(thread_t Callback, uint64_t CallbackArg, uint64_t GP0, uint64_t GP1, uint64_t GP2, uint64_t GP3);

KResult WindowClose(thread_t Callback, uint64_t CallbackArg, windowc* Window, uint64_t GP0, uint64_t GP1, uint64_t GP2);
KResult WindowResize(thread_t Callback, uint64_t CallbackArg, windowc* Window, uint64_t GP0, uint64_t GP1, uint64_t GP2);
KResult WindowChangePostion(thread_t Callback, uint64_t CallbackArg, windowc* Window, uint64_t GP0, uint64_t GP1, uint64_t GP2);
KResult WindowChangeVisibility(thread_t Callback, uint64_t CallbackArg, windowc* Window, uint64_t GP0, uint64_t GP1, uint64_t GP2);