#ifndef _MODULE_STORAGE_PARTITION_MBR_H
#define _MODULE_STORAGE_PARTITION_MBR_H

#include <main.h>

#define MBR_SIGNATURE 0xAA55

typedef struct{
    uint8_t boot_indicator;
    uint8_t start_head;
    uint8_t start_sector;
    uint8_t start_track;
    uint8_t os_indicator;
    uint8_t end_head;
    uint8_t end_sector;
    uint8_t end_track;
    uint32_t starting_lba;
    uint32_t size_in_lba;
}__attribute__((packed)) mbr_partition_entry_t;

typedef struct{
    uint8_t boot_code[440];
    uint32_t unique_mbr_disk_signature;
    uint16_t unknown;
    mbr_partition_entry_t partition_record[4];
    uint16_t signature;
}__attribute__((packed)) mbr_header_t;

int load_mbr_header(struct device_partitions_t* device_partition);

bool is_mbr_disk(struct device_partitions_t* device_partition);

bool is_gpt_disk(struct device_partitions_t* device_partition);

#endif // _MODULE_STORAGE_PARTITION_MBR_H