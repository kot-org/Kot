#pragma once

#include <core/main.h>

#define Shell_Srv_Version 0x1

typedef KResult (*shell_dispatch_t)(thread_t Callback, uint64_t CallbackArg, struct shell_t* File, uint64_t GP0, uint64_t GP1, uint64_t GP2);

KResult SrvInitalize();

KResult OpenShell(thread_t Callback, uint64_t CallbackArg, char* Path, permissions_t Permissions, process_t Target);

KResult ShellDispatch(thread_t Callback, uint64_t CallbackArg, uint64_t GP0, uint64_t GP1, uint64_t GP2, uint64_t GP3);
KResult Closeshell(thread_t Callback, uint64_t CallbackArg, struct shell_t* Shell, uint64_t GP0, uint64_t GP1, uint64_t GP2);
KResult Getshellsize(thread_t Callback, uint64_t CallbackArg, struct shell_t* Shell, uint64_t GP0, uint64_t GP1, uint64_t GP2);
KResult Readshell(thread_t Callback, uint64_t CallbackArg, struct shell_t* Shell, uint64_t GP0, uint64_t GP1, uint64_t GP2);
KResult Writeshell(thread_t Callback, uint64_t CallbackArg, struct shell_t* Shell, uint64_t GP0, uint64_t GP1, uint64_t GP2);