#include <keyhole/keyhole.h>


static locker_t mutexKeyhole;

KResult Keyhole_GetAddressFromIndex(key_t* key, uint64_t index, kprocess_t* parent){
    if(index > parent->LockIndex) return KFAIL;

    uint64_t Address = (index * sizeof(uint64_t)) + (uint64_t)parent->Locks;
    uint64_t Page = Address - (Address % PAGE_SIZE);
    uint64_t Offset = (Address % PAGE_SIZE) / sizeof(uint64_t);
    lockreference_t* AccessAddress = NULL;
    
    if(!vmm_GetFlags(parent->SharedPaging, (void*)Page, vmm_flag::vmm_Present)){
        return KFAIL;
    }else{
        AccessAddress = (lockreference_t*)vmm_GetVirtualAddress(vmm_GetPhysical(parent->SharedPaging, (void*)Page));
    }
    *key = (key_t)AccessAddress->LockOffset[Offset];
    return KSUCCESS;
}

KResult Keyhole_Create(key_t* key, kprocess_t* parent, kprocess_t* target, enum DataType type, uint64_t data, uint64_t flags, enum Priviledge minpriviledge, bool islocal){
    if(!CheckAddress((void*)key, sizeof(key))) return KFAIL;
    
    AtomicAcquire(&mutexKeyhole);
    
    parent->LockIndex++;
    // alloc lock
    lock_t* Lock = (lock_t*)kmalloc(sizeof(lock_t));
    // add reference to the parent
    uint64_t Address = (parent->LockIndex * sizeof(uint64_t)) + (uint64_t)parent->Locks;
    uint64_t Page = Address - (Address % PAGE_SIZE);
    uint64_t Offset = (Address % PAGE_SIZE) / sizeof(uint64_t);
    lockreference_t* AccessAddress = NULL;
    
    if(!vmm_GetFlags(parent->SharedPaging, (void*)Page, vmm_flag::vmm_Present)){
        vmm_Map(parent->SharedPaging, (void*)Page, Pmm_RequestPage());
        AccessAddress = (lockreference_t*)vmm_GetVirtualAddress(vmm_GetPhysical(parent->SharedPaging, (void*)Page));
        memset((void*)AccessAddress, 0, PAGE_SIZE);
    }else{
        AccessAddress = (lockreference_t*)vmm_GetVirtualAddress(vmm_GetPhysical(parent->SharedPaging, (void*)Page));
    }
    AccessAddress->LockOffset[Offset] = Lock;
    
    // fill data
    flags |= ~(1 << KeyholeFlagOriginal);
    Lock->Index = parent->LockIndex;
    Lock->Type = type;
    Lock->Target = target;
    Lock->Address = Address;
    Lock->Data = data;
    Lock->Parent = parent;
    Lock->Flags = flags;
    Lock->MinPriviledge = minpriviledge;
    
    // signature
    Lock->Signature0 = 'L';
    Lock->Signature1 = 'O';
    Lock->Signature2 = 'K';
    // setup key data
    if(islocal){
        *key = (uint64_t)Lock->Index;
    }else{
        *key = (uint64_t)Lock;
    }
    AtomicRelease(&mutexKeyhole);
    return KSUCCESS;
}

KResult Keyhole_CloneModify(kthread_t* caller, key_t key, key_t* newKey, kprocess_t* target, uint64_t flags, enum Priviledge minpriviledge, bool islocal){
    if(islocal){
        key_t TmpKey = key;
        if(Keyhole_GetAddressFromIndex(&key, TmpKey, caller->Parent) != KSUCCESS) return KKEYVIOLATION;
    } 

    lock_t* data;
    if(Keyhole_Get(caller, key, DataTypeUnknow, &data, false) != KSUCCESS) return KKEYVIOLATION;
    lock_t* lock = (lock_t*)key;
    if(!(data->Flags & KeyholeFlagCloneable)) return KKEYVIOLATION;
    if(flags & KeyholeFlagPresent){
        if(!(data->Flags & KeyholeFlagEditable)){
            return KKEYVIOLATION;
        }
    }else{
        flags = data->Flags;
    }

    flags &= ~(1 << KeyholeFlagOriginal);

    return Keyhole_Create(newKey, data->Parent, target, data->Type, data->Data, flags, minpriviledge, false);
}

KResult Keyhole_Verify(kthread_t* caller, key_t key, enum DataType type, bool islocal){
    if(islocal){
        key_t TmpKey = key;
        if(Keyhole_GetAddressFromIndex(&key, TmpKey, caller->Parent) != KSUCCESS) return KKEYVIOLATION;
    }

    // Get lock
    lock_t* lock = (lock_t*)key;
    // check lock
    if(!CheckAddress((void*)lock, sizeof(lock_t))) return KFAIL;
    if(lock->Signature0 != 'L' || lock->Signature1 != 'O' || lock->Signature2 != 'K') return KFAIL;
    if(lock->Target != NULL){
        if(lock->Target->PID != caller->Parent->PID) return KFAIL; // Just check the PID, to share key beetween fork process
    }
    if(type != DataTypeUnknow){
        if(lock->Type != type) return KFAIL;            
    }
    if(caller->Priviledge > lock->MinPriviledge){
        return KFAIL;
    }
    // check parent
    if(!CheckAddress((void*)lock->Parent, sizeof(kprocess_t))) return KFAIL;
    
    uint64_t VirtualAddress = (uint64_t)vmm_GetVirtualAddress(lock->Parent->SharedPaging);
    
    uint64_t PageAddress = lock->Address - (lock->Address % PAGE_SIZE);
    uint64_t Offset = (lock->Address % PAGE_SIZE) / sizeof(uint64_t);
    if(!vmm_GetFlags(lock->Parent->SharedPaging, (void*)PageAddress, vmm_flag::vmm_Present)) return KFAIL;
    lockreference_t* AccessAddress = (lockreference_t*)(vmm_GetVirtualAddress(vmm_GetPhysical(lock->Parent->SharedPaging, (void*)PageAddress)));
    return KSUCCESS;
}

KResult Keyhole_Get(kthread_t* caller, key_t key, enum DataType type, uint64_t* data, uint64_t* flags, bool islocal){       
    if(islocal){
        key_t TmpKey = key;
        if(Keyhole_GetAddressFromIndex(&key, TmpKey, caller->Parent) != KSUCCESS) return KKEYVIOLATION;
    }
      
    uint64_t Status = Keyhole_Verify(caller, key, type, false);
    if(Status != KSUCCESS){
        return Status;
    }
    lock_t* lock = (lock_t*)key;
    *data = lock->Data;
    *flags = lock->Flags;
    return KSUCCESS;
}

KResult Keyhole_Get(kthread_t* caller, key_t key, enum DataType type, lock_t** lock, bool islocal){        
    uint64_t Status = Keyhole_Verify(caller, key, type, islocal);
    if(Status != KSUCCESS){
       return Status; 
    } 
    *lock = (lock_t*)key;
    return KSUCCESS;
}