#include <main.h>
#include <device.h>

static inline void ahci_device_read_prepare(ahci_device_t* device, size_t* size_to_process, uint64_t* read_position, uintptr_t* buffer_position){
    size_t size_processing;
    uint64_t alignement_offset = *read_position % (uint64_t)device->alignement;

    if(alignement_offset){
        size_processing = device->alignement - alignement_offset;
        if(size_processing > *size_to_process){
            size_processing = *size_to_process;
        }
    }else{
        size_processing = *size_to_process;
    }
    

    if(size_processing > device->block_cache_size){
        size_processing = device->block_cache_size;
    }

    spinlock_acquire(&device->lock);
    device->read(device, *read_position - alignement_offset, size_processing, device->block_cache);
    memcpy((void*)*buffer_position, device->block_cache + alignement_offset, size_processing);
    spinlock_release(&device->lock);

    *buffer_position += (uintptr_t)size_processing;
    *read_position += (uint64_t)size_processing;
    *size_to_process -= size_processing;
}

static inline void ahci_device_write_prepare(ahci_device_t* device, size_t* size_to_process, uint64_t* write_position, uintptr_t* buffer_position){
    size_t size_processing;
    uint64_t alignement_offset = *write_position % (uint64_t)device->alignement;
    uint64_t start = *write_position - alignement_offset;

    if(alignement_offset){
        size_processing = device->alignement - alignement_offset;
        if(size_processing > *size_to_process){
            size_processing = *size_to_process;
        }
        ahci_device_read(device->storage, start, alignement_offset, device->block_cache);
    }else{
        size_processing = *size_to_process;
    }
    

    if(size_processing > device->block_cache_size){
        size_processing = device->block_cache_size;
    }else{
        uint64_t alignement_offset_end = (*write_position + size_processing) % (uint64_t)device->alignement;
        if(alignement_offset_end){
            ahci_device_read(device->storage, *write_position + size_processing, device->alignement - alignement_offset_end, (void*)((uintptr_t)device->block_cache + (uintptr_t)alignement_offset_end));
        }
    }

    spinlock_acquire(&device->lock);
    memcpy(device->block_cache + alignement_offset, (void*)*buffer_position, size_processing);

    size_t size_to_write = alignement_offset + size_processing;
    if(size_to_write % device->alignement){
        size_to_write -= size_to_write % device->alignement;
        size_to_write += device->alignement;
    }
    
    device->write(device, start, size_processing, device->block_cache);
    spinlock_release(&device->lock);

    *buffer_position += (uintptr_t)size_processing;
    *write_position += (uint64_t)size_processing;
    *size_to_process -= size_processing;
}

int ahci_device_read(struct storage_device_t* storage, uint64_t start, size_t size, void* buffer){
    ahci_device_t* device = storage->internal_data;

    uintptr_t buffer_position = (uintptr_t)buffer;
    uint64_t read_position = start;

    if(start + size > device->size){
        size = device->size - start;
    }

    for(size_t size_to_process = size; size_to_process > 0;){
        ahci_device_read_prepare(device, &size_to_process, &read_position, &buffer_position);
    }

    return 0;
}

int ahci_device_write(struct storage_device_t* storage, uint64_t start, size_t size, void* buffer){
    ahci_device_t* device = storage->internal_data;

    uintptr_t buffer_position = (uintptr_t)buffer;
    uint64_t write_position = start;

    if(start + size > device->size){
        size = device->size - start;
    }

    for(size_t size_to_process = size; size_to_process > 0;){
        ahci_device_write_prepare(device, &size_to_process, &write_position, &buffer_position);
    }

    return 0;
}

void ahci_init_device(ahci_device_t* device){
    device->storage = malloc(sizeof(storage_device_t));
    device->storage->storage_size = device->size;
    device->storage->internal_data = device;
    device->storage->read = &ahci_device_read;
    device->storage->write = &ahci_device_write;

    storage_handler->add_storage_device(device->storage);
}