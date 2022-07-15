#pragma once

#include <kot/sys.h>
#include <kot/keyhole.h>
#include <kot/modules/ipc/ipc.h>

kthread_t IPCInitialize();

KResult IPCHandler(struct IPCParameters_t data);