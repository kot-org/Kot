#pragma once

#include <core/main.h>
#include <kot/uisd/srvs/storage.h>

#define GUIDToListenSize 0x1

void InitializeVolumeListener();
void ListenerEvent(uint64_t VolumeID);