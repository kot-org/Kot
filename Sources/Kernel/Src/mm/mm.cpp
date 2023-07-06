#include <mm/mm.h>
#include <abi-bits/vm-flags.h>

static KResult AddRegion(MemoryHandler_t* Handler, MemoryRegion_t* Region){
    Region->Index = kot_vector_push(Handler->Regions, (void*)Region);
    return KSUCCESS;
}

static KResult RemoveRegion(MemoryHandler_t* Handler, MemoryRegion_t* Region){
    kot_vector_remove(Handler->Regions, Region->Index);
    Region->Index = -1;
    return KSUCCESS;
}

static MemoryRegion_t* SplitRegion(MemoryHandler_t* Handler, MemoryRegion_t* Region, size_t Size){
    if(Region->Size == Size){
        return Region;
    }else{
        MemoryRegion_t* NewRegion = (MemoryRegion_t*)kmalloc(sizeof(MemoryRegion_t));
        NewRegion->Base = Region->Base;
        NewRegion->Size = Size;
        NewRegion->BlockCount = Size / BLOCK_SIZE;
        NewRegion->Flags = Region->Flags;
        NewRegion->Prot = Region->Prot;
        NewRegion->IsFree = Region->IsFree;
        NewRegion->IsMap = Region->IsMap;
        AddRegion(Handler, NewRegion);

        Region->Base = (void*)((uintptr_t)Region->Base + Size);
        Region->Size -= Size;
        Region->BlockCount = Region->Size / BLOCK_SIZE;

        return NewRegion;
    }
}

MemoryHandler_t* MMCreateHandler(pagetable_t Paging, void* Base, size_t Size){
    MemoryHandler_t* Handler = (MemoryHandler_t*)kmalloc(sizeof(MemoryHandler_t));
    Handler->Base = Base;
    Handler->Size = Size;
    Handler->Paging = Paging;
    Handler->Regions = kot_vector_create();

    MemoryRegion_t* Region = (MemoryRegion_t*)kmalloc(sizeof(MemoryRegion_t));
    Region->Base = Base;
    Region->Size = Size;
    Region->BlockCount = Size / BLOCK_SIZE;
    Region->Flags = MAP_ANONYMOUS | MAP_FIXED;
    Region->Prot = PROT_NONE;
    Region->IsFree = true;
    Region->IsMap = false;

    AddRegion(Handler, Region);

    AtomicClearLock(&Handler->Lock);
    return Handler;
}

MemoryHandler_t* MMCloneHandler(pagetable_t Paging, MemoryHandler_t* Source){
    MemoryHandler_t* Destination = (MemoryHandler_t*)kmalloc(sizeof(MemoryHandler_t));
    Destination->Base = Source->Base;
    Destination->Size = Source->Size;
    Destination->Paging = Paging;
    Destination->Regions = kot_vector_create();

    for(uint64_t i = 0; i < Source->Regions->length; i++){
        MemoryRegion_t* Region = (MemoryRegion_t*)kot_vector_get(Source->Regions, i);
        MemoryRegion_t* RegionCopy = (MemoryRegion_t*)kmalloc(sizeof(MemoryRegion_t));

        memcpy(RegionCopy, Region, sizeof(MemoryRegion_t));

        AddRegion(Destination, RegionCopy);
    }    

    AtomicClearLock(&Destination->Lock);
    return Destination;    
}

KResult kfreeHandler(MemoryHandler_t* Handler){
    for(uint64_t i = 0; i < Handler->Regions->length; i++){
        MemoryRegion_t* Region = (MemoryRegion_t*)kot_vector_get(Handler->Regions, i);
        kfree(Region);
    }
    kot_vector_clear(Handler->Regions);
    kfree(Handler);
    return KSUCCESS;
}


MemoryRegion_t* MMAllocateRegionVM(MemoryHandler_t* Handler, void* Base, size_t Size, int Flags, int Prot){
    assert(!((uintptr_t)Base % PAGE_SIZE));
    assert(!(Size % PAGE_SIZE));

    AtomicAcquire(&Handler->Lock);

    MemoryRegion_t* Region = NULL;

    if(Flags & MAP_FIXED){
        MemoryRegion_t* Tmp = MMGetRegion(Handler, Base);
        if(!Tmp){
            AtomicRelease(&Handler->Lock);
            return NULL;
        }
        if(!Tmp->IsFree){
            AtomicRelease(&Handler->Lock);
            return NULL;
        }
        if(Tmp->Base != Base){
            size_t SizeUnusable = (uintptr_t)Base - (uintptr_t)Tmp->Base;
            if((Tmp->Size - SizeUnusable) < Size){
                AtomicRelease(&Handler->Lock);
                return NULL;
            }
            MemoryRegion_t* UnusableRegion = SplitRegion(Handler, Tmp, SizeUnusable);
            Region = SplitRegion(Handler, Tmp, Size);
        }else{
            if(Tmp->Size < Size){
                AtomicRelease(&Handler->Lock);
                return NULL;
            }
            Region = SplitRegion(Handler, Tmp, Size);
        }
    }else{
        MemoryRegion_t* RegionToSplit = NULL; 
        for(uint64_t i = 0; i < Handler->Regions->length; i++){
            MemoryRegion_t* Tmp = (MemoryRegion_t*)kot_vector_get(Handler->Regions, i);
            if(Tmp->IsFree){
                if(Tmp->Size >= Size){
                    RegionToSplit = Tmp;
                    break;
                }
            }
        }
        if(!RegionToSplit){
            AtomicRelease(&Handler->Lock);
            return NULL;
        }
        Region = SplitRegion(Handler, RegionToSplit, Size);
    }

    Region->IsFree = false;
    Region->Prot = Prot;
    Region->Flags = Flags;

    AtomicRelease(&Handler->Lock);

    return Region;
}

KResult kfreeRegionVM(MemoryHandler_t* Handler, MemoryRegion_t* Region, size_t Size){
    AtomicAcquire(&Handler->Lock);
    Region->IsFree = true;
    AtomicRelease(&Handler->Lock);
    return KSUCCESS;
}

MemoryRegion_t* MMGetRegion(MemoryHandler_t* Handler, void* Base){
    for(uint64_t i = 0; i < Handler->Regions->length; i++){
        MemoryRegion_t* Tmp = (MemoryRegion_t*)kot_vector_get(Handler->Regions, i);
        if((uintptr_t)Tmp->Base <= (uintptr_t)Base && (uintptr_t)Tmp->Base + Tmp->Size > (uintptr_t)Base){
            return Tmp;
        }
    }
    return NULL;
}


KResult MMAllocateMemoryBlock(MemoryHandler_t* Handler, MemoryRegion_t* Region){
    AtomicAcquire(&Handler->Lock);

    if(Region->IsFree){
        AtomicRelease(&Handler->Lock);
        return KFAIL;
    }


    if(Region->IsMap){
        AtomicRelease(&Handler->Lock);
        return KFAIL;
    }

    uintptr_t VirtualAddress = (uintptr_t)Region->Base;
    for(uint64_t i = 0; i < Region->BlockCount; i++){
        vmm_Map(Handler->Paging, (void*)VirtualAddress, Pmm_RequestPage(), Region->Prot & PROT_READ, Region->Prot & PROT_WRITE, true);
        VirtualAddress += BLOCK_SIZE;
    }

    Region->IsMap = true;

    AtomicRelease(&Handler->Lock);

    return KSUCCESS;
}

KResult MMAllocateMemoryContigous(MemoryHandler_t* Handler, MemoryRegion_t* Region){
    AtomicAcquire(&Handler->Lock);

    if(Region->IsFree){
        AtomicRelease(&Handler->Lock);
        return KFAIL;
    }


    if(Region->IsMap){
        AtomicRelease(&Handler->Lock);
        return KFAIL;
    }


    uintptr_t VirtualAddress = (uintptr_t)Region->Base;
    uintptr_t PhysicalAddress = (uintptr_t)Pmm_RequestPages(Region->BlockCount);
    for(uint64_t i = 0; i < Region->BlockCount; i++){
        vmm_Map(Handler->Paging, (void*)VirtualAddress, (void*)PhysicalAddress, Region->Prot & PROT_READ, Region->Prot & PROT_WRITE, true);
        VirtualAddress += BLOCK_SIZE;
        PhysicalAddress += BLOCK_SIZE;
    }

    Region->IsMap = true;

    AtomicRelease(&Handler->Lock);

    return KSUCCESS;
}

KResult MMMapPhysical(MemoryHandler_t* Handler, MemoryRegion_t* Region, void* Base){
    AtomicAcquire(&Handler->Lock);

    if(Region->IsFree){
        AtomicRelease(&Handler->Lock);
        return KFAIL;
    }

    if(Region->IsMap){
        AtomicRelease(&Handler->Lock);
        return KFAIL;
    }

    uintptr_t VirtualAddress = (uintptr_t)Region->Base;
    uintptr_t PhysicalAddress = (uintptr_t)Base;
    for(uint64_t i = 0; i < Region->BlockCount; i++){
        vmm_Map(Handler->Paging, (void*)VirtualAddress, (void*)PhysicalAddress, Region->Prot & PROT_READ, Region->Prot & PROT_WRITE, true);
        VirtualAddress += BLOCK_SIZE;
        PhysicalAddress += BLOCK_SIZE;
    }

    Region->IsMap = true;

    AtomicRelease(&Handler->Lock);

    return KSUCCESS;
}

KResult MMUnmap(MemoryHandler_t* Handler, MemoryRegion_t* Region){
    AtomicAcquire(&Handler->Lock);

    if(Region->IsFree){
        AtomicRelease(&Handler->Lock);
        return KFAIL;
    }

    if(Region->IsMap){
        AtomicRelease(&Handler->Lock);
        return KFAIL;
    }

    uintptr_t VirtualAddress = (uintptr_t)Region->Base;
    for(uint64_t i = 0; i < Region->BlockCount; i++){
        if(vmm_GetFlags(Handler->Paging, (void*)VirtualAddress, vmm_flag::vmm_Master)){
            if(vmm_GetFlags(Handler->Paging, (void*)VirtualAddress, vmm_flag::vmm_IsPureMemory)){
                Pmm_FreePage(vmm_GetPhysical(Handler->Paging, (void*)VirtualAddress));
            }
            vmm_Unmap(Handler->Paging, (void*)VirtualAddress);
        }
    }

    Region->IsMap = false;

    AtomicRelease(&Handler->Lock);

    return KSUCCESS;
}

KResult MMProtect(MemoryHandler_t* Handler, MemoryRegion_t* Region, int Prot){
    AtomicAcquire(&Handler->Lock);

    if(Region->IsFree){
        AtomicRelease(&Handler->Lock);
        return KFAIL;
    }


    Region->Prot = Prot;

    if(Region->IsMap){
        uintptr_t VirtualAddress = (uintptr_t)Region->Base;
        for(uint64_t i = 0; i < Region->BlockCount; i++){
            vmm_SetFlags(Handler->Paging, (void*)VirtualAddress, vmm_flag::vmm_User, Region->Prot & PROT_READ);
            vmm_SetFlags(Handler->Paging, (void*)VirtualAddress, vmm_flag::vmm_ReadWrite, Region->Prot & PROT_WRITE);
            VirtualAddress += BLOCK_SIZE;
        }
    }

    AtomicRelease(&Handler->Lock);

    return KSUCCESS;
}