#pragma once

#include "fs/vfs.h"
#include "drivers/gbd.h"

/**
 * Trivial Filesystem Implementation (https://kudos.readthedocs.io/en/latest/trivial-filesystem.html)
 * 
 * Currenly most of the stuff in the implementation is hidden, except for the init function.
 * 
 * See comments in the CPP file
 */

// Given a generic block device, and a starting sector, try to initialize
// a TFS implementation from the disk (This is used by the virtual filesystem)
fs_t* tfs_init(gbd_t* disk, uint32_t sector);