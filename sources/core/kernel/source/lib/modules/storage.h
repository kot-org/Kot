#ifndef _MODULES_STORAGE_H
#define _MODULES_STORAGE_H 1

#include <stdint.h>
#include <stddef.h>
#include <lib/guid.h>

typedef struct storage_device_t{
    int (*read)(struct storage_device_t*, uint64_t, size_t, void*);
    int (*write)(struct storage_device_t*, uint64_t, size_t, void*);
    uint64_t storage_size;
    void* internal_data;
    void* external_data; // reserved for storage handler
} storage_device_t;

typedef struct {
    int (*get_ownership)(storage_device_t*, uint64_t, uint64_t, guid_t*);
    void* external_data; // reserved for storage handler
} storage_potential_owner_t;

typedef struct storage_handler_t{
    int (*add_storage_device)(storage_device_t*);
    int (*remove_storage_device)(storage_device_t*);
    int (*add_potential_owner)(storage_potential_owner_t*);
    int (*remove_potential_owner)(storage_potential_owner_t*);
} storage_handler_t;

#endif // _MODULES_VFS_H