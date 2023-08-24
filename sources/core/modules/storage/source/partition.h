#ifndef _MODULE_STORAGE_PARTITION_H
#define _MODULE_STORAGE_PARTITION_H

struct device_partitions_t;

#include <main.h>
#include <partition/mbr.h>
#include <partition/gpt.h>
#include <global/modules.h>
#include <partition/crc32.h>

typedef struct device_partitions_t{
    // Device
    storage_device_t* device;

    // MBR
    bool is_mbr_header_loaded;
    mbr_header_t* mbr_header;

    // GPT
    bool is_gpt_header_loaded;
    bool is_gpt_partitions_loaded;
    gpt_header_t* gpt_header;
    gpt_partition_entry_t* gpt_partition_entries;
} device_partitions_t;

typedef struct {
    storage_device_t* device;
    uint64_t start;
    uint64_t size;
    guid_t partition_type_guid;
    bool is_owned;
    uint64_t vector_index;
} partition_t;

void init_partition(void);
int load_parititons(device_partitions_t* device_partitions);
int new_partition(storage_device_t* device, uint64_t start, uint64_t size, guid_t* partition_type_guid);

int add_potential_owner_storage(storage_potential_owner_t* potential_owner);
int remove_potential_owner_storage(storage_potential_owner_t* potential_owner);

static inline uint64_t bytes_to_lba(uint64_t value){
    return value >> 9;
}

static inline uint64_t lba_to_bytes(uint64_t value){
    return value << 9;
}

#endif // _MODULE_STORAGE_PARTITION_H
