#include <kot/descriptor.h>
#include <mlibc/debug.hpp>
#include <frg/string.hpp>
#include <frg/vector.hpp>
#include <bits/ensure.h>

static void* LastDescriptorMemoryAllocated = 0;
static size64_t LastDescriptorMemoryAllocatedSizeAvailable = 0;
static uint64_t DescriptorAllocateLock = 0;

static uintptr_t* DescriptorTable;
static uint64_t DescriptorTableLock = 0;

extern "C" {
    static void* kot_AllocateDescriptorMemory(size64_t Size){
        atomicAcquire(&DescriptorAllocateLock, 0);
        void* Buffer;
        if(LastDescriptorMemoryAllocatedSizeAvailable < Size){
            Buffer = kot_GetFreeAlignedSpace(KotSpecificData.MMapPageSize);
            size64_t SizeTmp = KotSpecificData.MMapPageSize;
            __ensure(kot_Sys_Map(kot_Sys_GetProcess(), &Buffer, AllocationTypeBasic, NULL, &SizeTmp, false) == KSUCCESS);
            LastDescriptorMemoryAllocatedSizeAvailable = KotSpecificData.MMapPageSize - Size;
            LastDescriptorMemoryAllocated = Buffer;
            if(DescriptorTable[KOT_DESCRIPTOR_MAP_DATA_INDEX] > (uintptr_t)LastDescriptorMemoryAllocated){
                DescriptorTable[KOT_DESCRIPTOR_MAP_DATA_INDEX] = (uintptr_t)LastDescriptorMemoryAllocated;
            }
        }else{
            LastDescriptorMemoryAllocatedSizeAvailable -= Size;
            LastDescriptorMemoryAllocated = (void*)((uintptr_t)LastDescriptorMemoryAllocated + Size);
            Buffer = LastDescriptorMemoryAllocated;
        }
        atomicUnlock(&DescriptorAllocateLock, 0);
        return Buffer;
    }

    int kot_InitializeDescriptorSaver(){
        size64_t Size = KOT_DESCRIPTOR_TABLE_SIZE;
        DescriptorTable = (uintptr_t*)kot_GetFreeAlignedSpace(Size);
        __ensure(kot_Sys_Map(kot_Sys_GetProcess(), (void**)&DescriptorTable, AllocationTypeBasic, NULL, &Size, false) == KSUCCESS);
        if(Size != 0){
            // Clear size allocate because if it's already allocate we don't want to earase descriptors already set
            memset(DescriptorTable, 0, Size);
            DescriptorTable[KOT_DESCRIPTOR_COUNT_INDEX] = (uintptr_t)KOT_DESCRIPTOR_FIRST_ALLOCATABLE_INDEX;
            DescriptorTable[KOT_DESCRIPTOR_MAP_DATA_INDEX] = (uintptr_t)DescriptorTable;
            return 0;
        }else{
            KotSpecificData.FreeMemorySpace = (void*)DescriptorTable[KOT_DESCRIPTOR_MAP_DATA_INDEX];
            return 1;
        }
    }

    int64_t kot_SaveDescriptor(kot_descriptor_t* Descriptor){
        if(!Descriptor){
            return -1;
        }

        if(DescriptorTable[DescriptorTable[KOT_DESCRIPTOR_COUNT_INDEX]] >= KOT_DESCRIPTOR_TABLE_COUNT){
            return -1;
        }

        void* DescriptorAddress = kot_AllocateDescriptorMemory(Descriptor->Size + sizeof(kot_descriptor_t));
        memcpy(DescriptorAddress, Descriptor, sizeof(kot_descriptor_t));
        memcpy((void*)((uintptr_t)DescriptorAddress + sizeof(kot_descriptor_t)), Descriptor->Data, Descriptor->Size);
        ((kot_descriptor_t*)DescriptorAddress)->Data = (void*)((uintptr_t)DescriptorAddress + sizeof(kot_descriptor_t));
        atomicAcquire(&DescriptorTableLock, 0);
        DescriptorTable[DescriptorTable[KOT_DESCRIPTOR_COUNT_INDEX]] = (uintptr_t)DescriptorAddress;
        uint64_t Index = DescriptorTable[KOT_DESCRIPTOR_COUNT_INDEX];
        DescriptorTable[KOT_DESCRIPTOR_COUNT_INDEX]++;
        atomicUnlock(&DescriptorTableLock, 0);


        // Start at 0 the index's descriptor gives to users
        return Index - KOT_DESCRIPTOR_FIRST_USABLE_INDEX; 
    }

    kot_descriptor_t* kot_GetDescriptor(int64_t Index){
        // Start at 1 the index's descriptor gives to us
        Index += KOT_DESCRIPTOR_FIRST_USABLE_INDEX;

        if(Index < KOT_DESCRIPTOR_FIRST_USABLE_INDEX){
            return NULL;
        }

        return (kot_descriptor_t*)DescriptorTable[Index];
    }

    KResult kot_ModifyDescriptor(int64_t Index, kot_descriptor_t* Descriptor){
        // Start at 1 the index's descriptor gives to us
        Index += KOT_DESCRIPTOR_FIRST_USABLE_INDEX;

        if(!Descriptor){
            return KFAIL;
        }

        if(DescriptorTable[Index]){
            if(((kot_descriptor_t*)DescriptorTable[Index])->Size == Descriptor->Size){
                memcpy((void*)DescriptorTable[Index], Descriptor, sizeof(kot_descriptor_t));
                memcpy((void*)((uintptr_t)DescriptorTable[Index] + sizeof(kot_descriptor_t)), Descriptor->Data, Descriptor->Size);
                ((kot_descriptor_t*)DescriptorTable[Index])->Data = (void*)((uintptr_t)DescriptorTable[Index] + sizeof(kot_descriptor_t));
                return KSUCCESS;
            }
        }
        
        void* DescriptorAddress = kot_AllocateDescriptorMemory(Descriptor->Size + sizeof(kot_descriptor_t));
        memcpy(DescriptorAddress, Descriptor, sizeof(kot_descriptor_t));
        memcpy((void*)((uintptr_t)DescriptorAddress + sizeof(kot_descriptor_t)), Descriptor->Data, Descriptor->Size);
        DescriptorTable[Index] = (uintptr_t)DescriptorAddress;
        ((kot_descriptor_t*)DescriptorAddress)->Data = (void*)((uintptr_t)DescriptorAddress + sizeof(kot_descriptor_t));
        // TODO : free the old one

        return KSUCCESS;
    }

    KResult kot_FreeDescriptor(int64_t Index){
        // Start at 1 the index's descriptor gives to us
        Index += KOT_DESCRIPTOR_FIRST_USABLE_INDEX;

        DescriptorTable[Index] = NULL;
        return KSUCCESS;
    }

    size64_t kot_GetDescriptorCount(){
        return static_cast<size64_t>(DescriptorTable[KOT_DESCRIPTOR_COUNT_INDEX]);
    }
}