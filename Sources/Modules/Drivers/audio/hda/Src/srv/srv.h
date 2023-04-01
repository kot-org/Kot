#pragma once

#include <core/main.h>

KResult InitializeSrv(struct HDAOutput* Output);

KResult ChangeStatus(thread_t Callback, uint64_t CallbackArg, enum AudioSetStatus Function, uint64_t GP0, uint64_t GP1, uint64_t GP2);