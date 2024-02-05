#ifndef _MODULE_STORAGE_PARTITION_GPT_H
#define _MODULE_STORAGE_PARTITION_GPT_H

#include <main.h>

#define GPT_MAX_PARTITIONS 0x80
#define GPT_SIGNATURE 0x5452415020494645
#define GPT_PARTITION_NAME_LEN   (72 / sizeof(uint16_t))

typedef struct{
    uint64_t signature;
    uint32_t revision;
    uint32_t header_size;
    uint32_t header_crc32;
    uint32_t reserved;
    uint64_t my_lba;
    uint64_t alternate_lba;
    uint64_t first_usable_lba;
    uint64_t last_usable_lba;
    guid_t disk_guid;
    uint64_t partition_entry_lba;
    uint32_t number_of_partition_entries;
    uint32_t size_of_partition_entry;
    uint32_t partition_entry_array_crc32;
}__attribute__((packed)) gpt_header_t;

typedef struct{
    guid_t partition_type_guid;
    guid_t unique_partition_guid;
    uint64_t starting_lba;
    uint64_t ending_lba;
    uint64_t attributes;
    uint16_t partition_name[GPT_PARTITION_NAME_LEN];
}__attribute__((packed)) gpt_partition_entry_t;

int load_gpt_partitions(struct device_partitions_t* device_partitions);

#endif // _MODULE_STORAGE_PARTITION_GPT_H