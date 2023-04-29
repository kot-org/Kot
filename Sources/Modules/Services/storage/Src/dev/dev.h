#pragma once

#include <core/main.h>
#include <partition/partition.h>

#define DEV_PATH "/dev/"
#define DEV_PATH_LEN strlen(DEV_PATH)

KResult InitializeDev();
KResult NewDev(thread_t Callback, uint64_t CallbackArg, uintptr_t Opaque);
KResult GetDevAccessData(char** RelativePath, struct partition_t** Partition, struct ClientVFSContext* Context, char* Path);