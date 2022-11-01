#pragma once

#include <core/main.h>
#include <kot/uisd/srvs/storage.h>
#include <kot/uisd/srvs/storage/device.h>

#include <kot++/printf.h>

#define GUIDToListenSize 0x1

void InitializeVolumeListener();
void ListenerEvent(uint64_t VolumeID, srv_storage_space_info_t* StorageSpace);