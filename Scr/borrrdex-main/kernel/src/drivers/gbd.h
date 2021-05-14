#pragma once

#ifndef __cplusplus
#error C++ only
#endif

#include "drivers/device.h"
#include <cstdint>

/**
 * This interface allows any device capable of storing blocks of data to be presented
 * in the same way to any filesystem.  It abstracts away the details of actually performing
 * reads, writes, etc.
 */ 

typedef enum {
    GBD_OPERATION_READ,
    GBD_OPERATION_WRITE
} gbd_operation_t;

/*
This struct is used to send requests to a block device
*/
typedef struct gbd_request_struct {
    // Start sector
    uint64_t start;

    // Number of sectors to read
    uint64_t count;

    // The RAM buffer for read/write
    void *buf;

    // What kind of operation (not used currently)
    gbd_operation_t operation;

    // Any internal context this request might have
    void* internal;

    // The next request to run after this one (useful for IDE, not used in AHCI)
    struct gbd_request_struct* next;

    // A return value for the operation (currently unused)
    int return_value;
} gbd_request_t;

/*
This struct represents a Generic Block Device (gbd), which is the representation
of a device which stores blocks of data that can be read from or written to.  Like
the filesystem struct, this struct has function pointers to wire up primitive functions.

For an implementation see drivers/ahci/AHCIController.cpp
*/
typedef struct gbd_struct {
    // This is the device that the GBD abstracts
    device_t* device;

    // Internal context for the GBD to use
    void* context;

    // Read block(s) of data according to the request
    int (*read_block)(struct gbd_struct *gbd, gbd_request_t *request);

    // Write block(s) of data according to the request
    int (*write_block)(struct gbd_struct *gbd, gbd_request_t *request);

    // Return the size of the blocks (sector size)
    uint32_t (*block_size)(struct gbd_struct *gbd);

    // Return the number of blocks in the device (sector count)
    uint64_t (*total_blocks)(struct gbd_struct *gbd);
} gbd_t;