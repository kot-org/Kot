#include <partition.h>

int load_mbr_header(device_partitions_t* device_partition){
    if(device_partition->mbr_header == NULL){
        device_partition->mbr_header = (mbr_header_t*)calloc(1, sizeof(mbr_header_t));
        device_partition->device->read(device_partition->device, 0, sizeof(mbr_header_t), device_partition->mbr_header);
    }
    return 0;
}

bool is_mbr_disk(device_partitions_t* device_partition){
    if(!device_partition->is_mbr_header_loaded){
        if(load_mbr_header(device_partition)){
            return false;
        }
    }
    return (device_partition->mbr_header->signature == MBR_SIGNATURE);
}

bool is_gpt_disk(device_partitions_t* device_partition){
    if(!device_partition->is_mbr_header_loaded){
        if(load_mbr_header(device_partition)){
            return false;
        }
    }

    if(is_mbr_disk(device_partition)){
        // check if MBR protective
        if(device_partition->mbr_header->partition_record[0].os_indicator == 0xEE){
            return true;
        }
    }
    
    return false;
}