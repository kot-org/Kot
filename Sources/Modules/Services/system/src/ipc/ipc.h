#pragma once

#include <kot/sys.h>
#include <kot/keyhole.h>
#include <kot/modules/ipc/ipc.h>

kthread_t IPCInitialize();

KResult IPCHandler(uint64_t IPCTask, uint64_t GlobalPurpose, uint64_t EncodedName);