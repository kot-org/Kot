#pragma once

#include <core/main.h>
#include <kot/types.h>

#define Time_Srv_Version 0x1

KResult InitialiseServer();

KResult SetTimePointerKeySrv(thread_t Callback, uint64_t CallbackArg, ksmem_t TimePointerKey);
KResult SetTickPointerKeySrv(thread_t Callback, uint64_t CallbackArg, ksmem_t TickPointerKey, uint64_t TickPeriod);