#pragma once

#include <core/main.h>

extern process_t VFSProcess;

KResult InitializeVFS();

KResult VFSMount(thread_t Callback, uint64_t CallbackArg, bool IsMount, srv_storage_fs_server_functions_t* StorageFSServerFunctions);