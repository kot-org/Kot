#include <partition.h>

static int check_gpt_header(gpt_header_t* gpt_header){
    if(gpt_header->signature == GPT_SIGNATURE){
        uint32_t crc32_header_tmp = gpt_header->header_crc32;
        gpt_header->header_crc32 = 0;
        uint32_t crc32_header_compute = partition_crc32(0, (char*)gpt_header, sizeof(gpt_header_t));
        gpt_header->header_crc32 = crc32_header_tmp;

        if(gpt_header->header_crc32 == crc32_header_compute){
            return 0;
        }else{
            return EINVAL;
        }
    }

    return EINVAL;
}

static int load_gpt_header(device_partitions_t* device_partitions){
    assert(device_partitions->gpt_header == NULL);
    device_partitions->gpt_header = (gpt_header_t*)malloc(sizeof(gpt_header_t));

    assert(device_partitions->mbr_header != NULL);
    uint64_t gpt_header_lba_start = device_partitions->mbr_header->partition_record[0].starting_lba;
    device_partitions->device->read(device_partitions->device, lba_to_bytes(gpt_header_lba_start), sizeof(gpt_header_t), (void*)device_partitions->gpt_header);
    if(device_partitions->gpt_header->my_lba != gpt_header_lba_start){
        return EINVAL;
    }
    int gpt_header_status = check_gpt_header(device_partitions->gpt_header);
    // load recovery header
    gpt_header_t* recovery_gpt_header = (gpt_header_t*)malloc(sizeof(gpt_header_t));
    device_partitions->device->read(device_partitions->device, lba_to_bytes(device_partitions->gpt_header->alternate_lba), sizeof(gpt_header_t), (void*)recovery_gpt_header);
    int gpt_recovery_header_status = check_gpt_header(recovery_gpt_header);

    if(gpt_header_status){
        if(!gpt_recovery_header_status){
            // update my lba
            recovery_gpt_header->partition_entry_lba = recovery_gpt_header->my_lba - recovery_gpt_header->partition_entry_lba + gpt_header_lba_start; 
            recovery_gpt_header->my_lba = gpt_header_lba_start; 

            // update checksum
            recovery_gpt_header->header_crc32 = 0;
            recovery_gpt_header->header_crc32 = partition_crc32(0, (char*)recovery_gpt_header, sizeof(gpt_header_t));

            // update gpt header into the disk
            device_partitions->device->write(device_partitions->device, lba_to_bytes(gpt_header_lba_start), sizeof(gpt_header_t), (void*)recovery_gpt_header);

            // update gpt header
            device_partitions->device->read(device_partitions->device, lba_to_bytes(gpt_header_lba_start), sizeof(gpt_header_t), (void*)device_partitions->gpt_header);
        }else{
            free(recovery_gpt_header);
            return EINVAL;
        }
    }else if(gpt_recovery_header_status){
        // update my lba
        recovery_gpt_header->my_lba = device_partitions->gpt_header->alternate_lba; 

        // update checksum
        recovery_gpt_header->header_crc32 = 0;
        recovery_gpt_header->header_crc32 = partition_crc32(0, (char*)recovery_gpt_header, sizeof(gpt_header_t));

        // update gpt recovery header into the disk
        device_partitions->device->write(device_partitions->device, lba_to_bytes(gpt_header_lba_start), sizeof(gpt_header_t), (void*)recovery_gpt_header);
    }

    if(!gpt_header_status){
        device_partitions->is_gpt_header_loaded = true;
        free(recovery_gpt_header);
        return 0;
    }
    free(recovery_gpt_header);
    return EINVAL;
}

static uint64_t check_partitions(device_partitions_t* device_partitions){
    size_t size_of_partition_list = sizeof(gpt_partition_entry_t) * device_partitions->gpt_header->number_of_partition_entries;

    uint32_t crc32_header_compute = partition_crc32(0, (char*)device_partitions->gpt_partition_entries, size_of_partition_list);

    // check recovery 
    gpt_partition_entry_t* gpt_partition_entries_recovery = (gpt_partition_entry_t*)malloc(size_of_partition_list);
    uint64_t pgt_partition_entries_revory_location = lba_to_bytes(device_partitions->gpt_header->alternate_lba) - size_of_partition_list;
    device_partitions->device->read(device_partitions->device, pgt_partition_entries_revory_location, size_of_partition_list, gpt_partition_entries_recovery);
    uint32_t crc32_recovery_header_compute = partition_crc32(0, (char*)gpt_partition_entries_recovery, size_of_partition_list);

    if(device_partitions->gpt_header->partition_entry_array_crc32 == crc32_header_compute){
        if(device_partitions->gpt_header->partition_entry_array_crc32 != crc32_recovery_header_compute){
            // update gpt recovery partition entries into the disk
            device_partitions->device->write(device_partitions->device, pgt_partition_entries_revory_location, size_of_partition_list, device_partitions->gpt_partition_entries);
        }
        free(gpt_partition_entries_recovery);
        return 0;
    }else{
        if(device_partitions->gpt_header->partition_entry_array_crc32 == crc32_recovery_header_compute){
            // update gpt partition entries into the disk
            device_partitions->device->write(device_partitions->device, lba_to_bytes(device_partitions->gpt_header->partition_entry_lba), size_of_partition_list, gpt_partition_entries_recovery);

            // update entries
            device_partitions->device->read(device_partitions->device, lba_to_bytes(device_partitions->gpt_header->partition_entry_lba), size_of_partition_list, device_partitions->gpt_partition_entries);

            free(gpt_partition_entries_recovery);
            return 0;
        }
    }

    return EINVAL;
}

int load_gpt_partitions(device_partitions_t* device_partitions){
    if(!device_partitions->is_gpt_header_loaded){
        if(load_gpt_header(device_partitions)){
            return EINVAL;
        }
    }
    
    // Load partitions
    size_t size_of_partition_list = sizeof(gpt_partition_entry_t) * device_partitions->gpt_header->number_of_partition_entries;
    assert(device_partitions->gpt_partition_entries == NULL);
    device_partitions->gpt_partition_entries = (gpt_partition_entry_t*)malloc(size_of_partition_list);

    device_partitions->device->read(device_partitions->device, lba_to_bytes(device_partitions->gpt_header->partition_entry_lba), size_of_partition_list, device_partitions->gpt_partition_entries);
    if(!check_partitions(device_partitions)){
        device_partitions->is_gpt_partitions_loaded = true;
        for(uint64_t i = 0; i < device_partitions->gpt_header->number_of_partition_entries; i++){
            uint64_t start = lba_to_bytes(device_partitions->gpt_partition_entries[i].starting_lba);
            uint64_t size = lba_to_bytes(device_partitions->gpt_partition_entries[i].ending_lba - device_partitions->gpt_partition_entries[i].starting_lba);
            if(size != 0){
                new_partition(device_partitions->device, start, size, &device_partitions->gpt_partition_entries[i].partition_type_guid);
            }
        }
        return 0;
    }
    
    return EINVAL;
}