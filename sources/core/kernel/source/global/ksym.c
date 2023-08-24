#include <stdint.h>
#include <lib/log.h>
#include <lib/assert.h>
#include <lib/string.h>
#include <impl/initrd.h>
#include <global/ksym.h>

static hashmap_t* ksym_hashmap = NULL;
static ksym_t* ksym_list = NULL;
static size_t ksym_size = 0;

void ksym_init(void){
	assert(ksym_hashmap == NULL);
	ksym_hashmap = hashmap_create(32);

	void* file_ptr = initrd_get_file("/kernel.sym");
	assert(file_ptr);

	ksym_list = initrd_get_file_base(file_ptr);
	ksym_size = initrd_get_file_size(file_ptr);

	ksym_t* ksym = ksym_list;
	for(size_t i = 0; i < ksym_size;){
	 	ksym_add(ksym->name, (void*)ksym->address);
		size_t size_field = sizeof(elf64_addr) + sizeof(elf64_xword) + strlen(ksym->name) + 1;
	 	ksym = (ksym_t*)((uintptr_t)ksym + (uintptr_t)size_field);
		i += size_field;
	}
}

void ksym_add(const char* symname, void* address){
	assert(ksym_hashmap != NULL);
	hashmap_set(ksym_hashmap, symname, address);
}

void* ksym_find(const char* symname){
	assert(ksym_hashmap != NULL);
	return hashmap_get(ksym_hashmap, symname);
}

char* ksym_get_name(void* address){
	assert(ksym_list != NULL);

	ksym_t* ksym = ksym_list;
	for(size_t i = 0; i < ksym_size;){
	 	if((uintptr_t)ksym->address <= (uintptr_t)address && (uintptr_t)ksym->address + (uintptr_t)ksym->size >= (uintptr_t)address){
			return ksym->name;
		}
		size_t size_field = sizeof(elf64_addr) + sizeof(elf64_xword) + strlen(ksym->name) + 1;
	 	ksym = (ksym_t*)((uintptr_t)ksym + (uintptr_t)size_field);
		i += size_field;
	}

	return NULL;
}

hashmap_t* ksym_get_map(void){
	return ksym_hashmap;
}