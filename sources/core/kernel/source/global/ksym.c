#include <stdint.h>
#include <lib/log.h>
#include <impl/vmm.h>
#include <lib/assert.h>
#include <lib/string.h>
#include <lib/vector.h>
#include <lib/hashmap.h>
#include <impl/initrd.h>
#include <global/ksym.h>

static hashmap_t* ksym_hashmap_kernel_shareable_symbols = NULL;
static vector_t* ksym_vector = NULL;
static spinlock_t ksym_lock = SPINLOCK_INIT;

void ksym_init(void){
	assert(ksym_hashmap_kernel_shareable_symbols == NULL);
	assert(ksym_vector == NULL);

	ksym_hashmap_kernel_shareable_symbols = hashmap_create(32);
	ksym_vector = vector_create();

	void* file_ptr = initrd_get_file("/system/kernel/kernel.sym");
	assert(file_ptr);

	ksym_t* ksym_list = initrd_get_file_base(file_ptr);
	size_t ksym_size = initrd_get_file_size(file_ptr);

	ksym_t* ksym = ksym_list;
	for(size_t i = 0; i < ksym_size;){
	 	ksym_add(ksym, true);
		size_t size_field = sizeof(ksym_t) + ksym->name_len + sizeof((char)'\0');
	 	ksym = (ksym_t*)((uintptr_t)ksym + (uintptr_t)size_field);
		i += size_field;
	}
}

void ksym_add(ksym_t* ksym, bool add_to_kernel_shareable_symbols){
	spinlock_acquire(&ksym_lock);

	if(add_to_kernel_shareable_symbols){
		assert(ksym_hashmap_kernel_shareable_symbols != NULL);
		hashmap_set(ksym_hashmap_kernel_shareable_symbols, ksym->name, (void*)ksym->address);
	}

	assert(ksym_vector != NULL);
	vector_push(ksym_vector, ksym);

	spinlock_release(&ksym_lock);
}

void* ksym_get_address_kernel_shareable_symbols(const char* symname){
	assert(ksym_hashmap_kernel_shareable_symbols != NULL);

	spinlock_acquire(&ksym_lock);

	void* address = hashmap_get(ksym_hashmap_kernel_shareable_symbols, symname);

	spinlock_release(&ksym_lock);

	return address;
}

char* ksym_get_name(void* address){
	if((uintptr_t)address <= (uintptr_t)VMM_USERSPACE_TOP_ADDRESS && (uintptr_t)address >= (uintptr_t)VMM_USERSPACE_BOTTOM_ADDRESS){
		return NULL;
	}

	assert(ksym_vector != NULL);

	spinlock_acquire(&ksym_lock);

	for(uint64_t i = 0; i < ksym_vector->length; i++){
		ksym_t* ksym = vector_get(ksym_vector, i);

	 	if((uintptr_t)ksym->address <= (uintptr_t)address && (uintptr_t)ksym->address + (uintptr_t)ksym->size >= (uintptr_t)address){
			spinlock_release(&ksym_lock);
			return ksym->name;
		}
	}

	spinlock_release(&ksym_lock);

	return NULL;
}