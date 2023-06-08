#pragma once

#include <core/main.h>

KResult InitializeSrv(struct HDAOutput* Output);

KResult ChangeStatus(kot_thread_t Callback, uint64_t CallbackArg, enum kot_AudioSetStatus Function, uint64_t GP0, uint64_t GP1, uint64_t GP2);