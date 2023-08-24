storage_potential_owner_t fat32_potential_owner;

int get_ownership(storage_device_t* device, uint64_t start, uint64_t size, guid_t* guid){
	partition_t* partition = malloc(sizeof(partition_t));
	partition->start = start;
	partition->size = size;
	partition->device = device;

	if(fat_mount(partition)){
		return 0;
	}

	free(partition);
	return EINVAL;
}

void init_interface(void){
	fat32_potential_owner.get_ownership = &get_ownership;
    storage_handler->add_potential_owner(&fat32_potential_owner);
}
