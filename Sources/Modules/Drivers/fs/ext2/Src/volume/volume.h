#pragma once

#include <srv/srv.h>
#include <main/main.h>
#include <core/core.h>
#include <stdio.h>
#include <kot/uisd/srvs/storage/device.h>

#define GUIDToListenSize 0x1

extern kot_process_t ProcessKey;

void InitializeVolumeListener();
void ListenerEvent(uint64_t VolumeID, struct srv_storage_space_info_t* StorageSpace, kot_process_t VFSProcess, kot_thread_t VFSConnect);