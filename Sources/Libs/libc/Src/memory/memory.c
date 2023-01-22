#include <kot/memory.h>

void memset(uintptr_t start, uint8_t value, size64_t size){
    for (uint64_t i = 0; i < size; i += sizeof(uint8_t)){
        *(uint8_t*)((uint64_t)start + i) = value;
    }
}       

void memset16(uintptr_t start, uint16_t value, size64_t size){
    for (uint64_t i = 0; i < size; i += sizeof(uint16_t)){
        *(uint16_t*)((uint64_t)start + i) = value;
    }
}

void memset32(uintptr_t start, uint32_t value, size64_t size){
    for (uint64_t i = 0; i < size; i += sizeof(uint32_t)){
        *(uint32_t*)((uint64_t)start + i) = value;
    }
}

void memset64(uintptr_t start, uint64_t value, size64_t size){
    for (uint64_t i = 0; i < size; i += sizeof(uint64_t)){
        *(uint64_t*)((uint64_t)start + i) = value;
    }
}

void memcpy(uintptr_t destination, uintptr_t source, size64_t size){
    long d0, d1, d2; 
    __asm__ volatile(
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

uint64_t MemoryLock;

uintptr_t GetFreeAlignedSpace(size64_t size){
    atomicAcquire(&MemoryLock, 0);
    if(size % KotSpecificData.MMapPageSize){
        size -= size % KotSpecificData.MMapPageSize;
        size += KotSpecificData.MMapPageSize;
    }
    KotSpecificData.FreeMemorySpace -= size;
    uintptr_t ReturnValue = KotSpecificData.FreeMemorySpace;
    atomicUnlock(&MemoryLock, 0);
    return ReturnValue;
}

uintptr_t MapPhysical(uintptr_t physicalAddress, size64_t size){
    uint64_t ReturnValue = (uint64_t)GetFreeAlignedSpace(size);
    Sys_Map(Sys_GetProcess(), (uintptr_t*)&ReturnValue, AllocationTypePhysical, &physicalAddress, &size, false);
    ReturnValue |= (uint64_t)physicalAddress & 0xFFF;
    return ReturnValue;
}

void MapPhysicalToVirtual(uintptr_t virtualAddress, uintptr_t* physicalAddress, size64_t size){
    Sys_Map(Sys_GetProcess(), (uintptr_t*)&virtualAddress, AllocationTypePhysicalContiguous, physicalAddress, &size, false);
}

uintptr_t GetPhysical(uintptr_t* physicalAddress, size64_t size){
    uint64_t ReturnValue = (uint64_t)GetFreeAlignedSpace(size);
    Sys_Map(Sys_GetProcess(), (uintptr_t*)&ReturnValue, AllocationTypePhysicalContiguous, physicalAddress, &size, false);
    return ReturnValue;
}

void FreeAddress(uintptr_t virtualAddress, size64_t size){
    Sys_Unmap(Sys_GetProcess(), virtualAddress, size);
}