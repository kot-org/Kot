#pragma once

#ifndef __cplusplus
#error C++ only
#endif

#include "vfs.h"
#include "drivers/gbd.h"
#include <cstdint>

typedef struct filesystems_struct {
    const char* name;

    fs_t* (*init)(gbd_t* disk, uint32_t sector);
} filesystems_t;

fs_t* filesystems_try_all(gbd_t* disk);