#include <main.h>
#include <partition.h>
#include <lib/assert.h>
#include <global/modules.h>

storage_handler_t storage_internal_handler;

int add_storage_device(storage_device_t* device){
    if(device == NULL){
        return EINVAL;
    }

    device_partitions_t* device_partitions = calloc(1, sizeof(device_partitions_t));
    device_partitions->device = device;

    assert(load_parititons(device_partitions) == 0);


    device->external_data = device_partitions;

    return 0;
}

int remove_storage_device(storage_device_t* device){
    if(device == NULL){
        return EINVAL;
    }

    if(device->external_data == NULL){
        return EINVAL;
    }

    return 0;
}

int add_potential_owner(storage_potential_owner_t* potential_owner){
    if(potential_owner == NULL){
        return EINVAL;
    }
    return add_potential_owner_storage(potential_owner);
}

int remove_potential_owner(storage_potential_owner_t* potential_owner){
    if(potential_owner == NULL){
        return EINVAL;
    }
    return remove_potential_owner_storage(potential_owner);
}

void interface_init(void){
    storage_internal_handler.add_storage_device = &add_storage_device;
    storage_internal_handler.remove_storage_device = &remove_storage_device;
    storage_internal_handler.add_potential_owner = &add_potential_owner;
    storage_internal_handler.remove_potential_owner = &remove_potential_owner;
    storage_handler = &storage_internal_handler;
}