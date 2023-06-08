#pragma once

#include <core/main.h>

#define Shell_Srv_Version 0x1

typedef KResult (*shell_dispatch_t)(kot_thread_t Callback, uint64_t CallbackArg, struct shell_t* File, uint64_t GP0, uint64_t GP1, uint64_t GP2);

KResult SrvInitalize();

KResult OpenShell(kot_thread_t Callback, uint64_t CallbackArg, char* Path, kot_permissions_t Permissions, kot_process_t Target);

KResult ShellDispatch(kot_thread_t Callback, uint64_t CallbackArg, uint64_t GP0, uint64_t GP1, uint64_t GP2, uint64_t GP3);
KResult Closeshell(kot_thread_t Callback, uint64_t CallbackArg, struct shell_t* Shell, uint64_t GP0, uint64_t GP1, uint64_t GP2);
KResult Getshellsize(kot_thread_t Callback, uint64_t CallbackArg, struct shell_t* Shell, uint64_t GP0, uint64_t GP1, uint64_t GP2);
KResult Readshell(kot_thread_t Callback, uint64_t CallbackArg, struct shell_t* Shell, uint64_t GP0, uint64_t GP1, uint64_t GP2);
KResult Writeshell(kot_thread_t Callback, uint64_t CallbackArg, struct shell_t* Shell, uint64_t GP0, uint64_t GP1, uint64_t GP2);