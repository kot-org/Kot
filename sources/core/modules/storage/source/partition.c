#include <partition.h>

static spinlock_t partition_list_lock = {};
static vector_t* partition_list = NULL;

static spinlock_t potential_owner_list_lock = {};
static vector_t* potential_owner_list = NULL;

static void add_partition_to_list(partition_t* partition){
    spinlock_acquire(&partition_list_lock);
    partition->vector_index = vector_push(partition_list, partition);
    spinlock_release(&partition_list_lock);
}

static void remove_partition_to_list(partition_t* partition){
    spinlock_acquire(&partition_list_lock);
    vector_remove(partition_list, partition->vector_index);
    spinlock_release(&partition_list_lock);
}

static void add_potential_owner_to_list(storage_potential_owner_t* potential_owner){
    spinlock_acquire(&potential_owner_list_lock);
    potential_owner->external_data = (void*)vector_push(potential_owner_list, potential_owner);
    spinlock_release(&potential_owner_list_lock);
}

static void remove_potential_owner_to_list(storage_potential_owner_t* potential_owner){
    spinlock_acquire(&potential_owner_list_lock);
    vector_remove(potential_owner_list, (uint64_t)potential_owner->external_data);
    spinlock_release(&potential_owner_list_lock);
}

static void find_partition_owner(partition_t* partition){
    spinlock_acquire(&potential_owner_list_lock);
    for(uint64_t i = 0; i < potential_owner_list->length; i++){
        storage_potential_owner_t* potential_owner = vector_get(potential_owner_list, i);
        partition->is_owned = (potential_owner->get_ownership(partition->device, partition->start, partition->size, &partition->partition_type_guid) == 0);
        if(partition->is_owned){
            break;
        }
    }
    spinlock_release(&potential_owner_list_lock);
}

static void find_owner_partitions(storage_potential_owner_t* potential_owner){
    spinlock_acquire(&partition_list_lock);
    for(uint64_t i = 0; i < partition_list->length; i++){
        partition_t* partition = vector_get(partition_list, i);
        if(!partition->is_owned){
            partition->is_owned = (potential_owner->get_ownership(partition->device, partition->start, partition->size, &partition->partition_type_guid) == 0);
        }
    }
    spinlock_release(&partition_list_lock);
}

void init_partition(void){
    partition_list = vector_create();
    potential_owner_list = vector_create();
}

int load_parititons(device_partitions_t* device_partitions){
    if(is_gpt_disk(device_partitions)){    
        return load_gpt_partitions(device_partitions);
    }

    return EIO;
}

int new_partition(storage_device_t* device, uint64_t start, uint64_t size, guid_t* partition_type_guid){
    partition_t* partition = malloc(sizeof(partition_t));
    partition->device = device;
    partition->start = start;
    partition->size = size;
    memcpy(&partition->partition_type_guid, partition_type_guid, sizeof(guid_t));
    partition->is_owned = false;

    add_partition_to_list(partition);
    find_partition_owner(partition);

    return 0;
}

int add_potential_owner_storage(storage_potential_owner_t* potential_owner){
    add_potential_owner_to_list(potential_owner);
    find_owner_partitions(potential_owner);
    return 0;
}

int remove_potential_owner_storage(storage_potential_owner_t* potential_owner){
    remove_potential_owner_to_list(potential_owner);
    return 0;
}