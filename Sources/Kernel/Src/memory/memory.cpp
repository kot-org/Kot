#include <arch/arch.h>
#include <logs/logs.h>
#include <memory/memory.h>

void memset(void* start, uint8_t value, size64_t size){
    for (uint64_t i = 0; i < size; i += sizeof(uint8_t)){
        *(uint8_t*)((uint64_t)start + i) = value;
    }
}       

void memset16(void* start, uint16_t value, size64_t size){
    for (uint64_t i = 0; i < size; i += sizeof(uint16_t)){
        *(uint16_t*)((uint64_t)start + i) = value;
    }
}

void memset32(void* start, uint32_t value, size64_t size){
    for (uint64_t i = 0; i < size; i += sizeof(uint32_t)){
        *(uint32_t*)((uint64_t)start + i) = value;
    }
}

void memset64(void* start, uint64_t value, size64_t size){
    for (uint64_t i = 0; i < size; i += sizeof(uint64_t)){
        *(uint64_t*)((uint64_t)start + i) = value;
    }
}

void memcpy(void* destination, void* source, size64_t size){
    long d0, d1, d2; 
    asm volatile(
            "rep ; movsq\n\t movq %4,%%rcx\n\t""rep ; movsb\n\t": "=&c" (d0),
            "=&D" (d1),
            "=&S" (d2): "0" (size >> 3), 
            "g" (size & 7), 
            "1" (destination),
            "2" (source): "memory"
    );  
}

int memcmp(const void *aptr, const void *bptr, size64_t size){
	const unsigned char *a = (const unsigned char*)aptr, *b = (const unsigned char*)bptr;
	for (size64_t i = 0; i < size; i++) {
		if (a[i] < b[i])
			return -1;
		else if (a[i] > b[i])
			return 1;
	}
	return 0;
}

bool CheckAddress(void* address, size64_t size, void* pagingEntry){
    if(address == NULL) return false;
    uint64_t NumberPage = DivideRoundUp(size, PAGE_SIZE);
    uint64_t AddressItinerator = (uint64_t)address;

    for(uint64_t i = 0; i < NumberPage; i++){
        if(!vmm_GetFlags(pagingEntry, (void*)AddressItinerator, vmm_flag::vmm_Present)){
            return false;
        } 
        AddressItinerator += PAGE_SIZE;
    }

    return true;
}

bool CheckAddress(void* address, size64_t size){
    return CheckAddress(address, size, ASMGetPagingEntry());
}

bool CheckUserAddress(void* address, size64_t size, void* pagingEntry){
    if(address == NULL) return false;
    if((uint64_t)address >= VMM_HIGHER_HALF_ADDRESS){
        return false;
    }
    uint64_t NumberPage = DivideRoundUp(size, PAGE_SIZE);
    uint64_t AddressItinerator = (uint64_t)address;

    for(uint64_t i = 0; i < NumberPage; i++){
        if(!vmm_GetFlags(pagingEntry, (void*)AddressItinerator, vmm_flag::vmm_Present)){
            return false;
        } 
        AddressItinerator += PAGE_SIZE;
    }

    return true;
}

bool CheckUserAddress(void* address, size64_t size){
    void* PagingEntry = NULL;
    __asm__ __volatile__ ("mov %%cr3, %%rax" : "=a"(PagingEntry));
    return CheckAddress(address, size, PagingEntry);
}