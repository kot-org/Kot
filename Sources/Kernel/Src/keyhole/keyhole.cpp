#include <keyhole/keyhole.h>


static uint64_t mutexKeyhole;

KResult Keyhole_Create(key_t* key, kprocess_t* parent, kprocess_t* target, enum DataType type, uint64_t data, uint64_t flags){
    if(!CheckAddress((uintptr_t)key, sizeof(key))) return KFAIL;
    
    Atomic::atomicAcquire(&mutexKeyhole, 0);
    
    parent->LockIndex++;
    // alloc lock
    lock_t* Lock = (lock_t*)malloc(sizeof(lock_t));
    // add reference to the parent
    uint64_t Address = (parent->LockIndex * sizeof(uint64_t)) + (uint64_t)parent->Locks;
    uint64_t Page = Address - (Address % PAGE_SIZE);
    uint64_t Offset = (Address % PAGE_SIZE) / sizeof(uint64_t);
    lockreference_t* AccessAddress = NULL;
    
    if(!vmm_GetFlags(parent->SharedPaging, (uintptr_t)Page, vmm_flag::vmm_Present)){
        vmm_Map(parent->SharedPaging, (uintptr_t)Page, Pmm_RequestPage());
        AccessAddress = (lockreference_t*)vmm_GetVirtualAddress(vmm_GetPhysical(parent->SharedPaging, (uintptr_t)Page));
        memset((uintptr_t)AccessAddress, 0, PAGE_SIZE);
    }else{
        AccessAddress = (lockreference_t*)vmm_GetVirtualAddress(vmm_GetPhysical(parent->SharedPaging, (uintptr_t)Page));
    }
    AccessAddress->LockOffset[Offset] = Lock;
    
    // fill data
    flags |= ~(1 << KeyholeFlagOriginal);
    Lock->Type = type;
    Lock->Target = target;
    Lock->Address = Address;
    Lock->Data = data;
    Lock->Parent = parent;
    Lock->Flags = flags;
    
    // signature
    Lock->Signature0 = 'L';
    Lock->Signature1 = 'O';
    Lock->Signature2 = 'K';
    // setup key data
    *key = (uint64_t)Lock;
    Atomic::atomicUnlock(&mutexKeyhole, 0);
    return KSUCCESS;
}

KResult Keyhole_CloneModify(kthread_t* caller, key_t key, key_t* newKey, kprocess_t* target, uint64_t flags){
    lock_t* data;
    if(Keyhole_Get(caller, key, DataTypeUnknow, &data) != KSUCCESS) return KKEYVIOLATION;
    lock_t* lock = (lock_t*)key;
    if(!(Keyhole_GetFlag(data->Flags, KeyholeFlagCloneable))) return KKEYVIOLATION;
    if(Keyhole_GetFlag(flags, KeyholeFlagPresent)){
        if(!(Keyhole_GetFlag(data->Flags, KeyholeFlagEditable))){
            return KKEYVIOLATION;
        }
    }else{
        flags = data->Flags;
    }

    flags &= ~(1 << KeyholeFlagOriginal);

    return Keyhole_Create(newKey, data->Parent, target, data->Type, data->Data, flags);
}

KResult Keyhole_Verify(kthread_t* caller, key_t key, enum DataType type){
    // get lock
    lock_t* lock = (lock_t*)key;
    // check lock
    if(!CheckAddress((uintptr_t)lock, sizeof(lock_t))) return KFAIL;
    if(lock->Signature0 != 'L' || lock->Signature1 != 'O' || lock->Signature2 != 'K') return KFAIL;
    if(lock->Target != NULL){
        if(lock->Target != caller->Parent) return KFAIL;
    }
    if(type != DataTypeUnknow){
        if(lock->Type != type) return KFAIL;            
    }
    // check parent
    if(!CheckAddress((uintptr_t)lock->Parent, sizeof(kprocess_t))) return KFAIL;
    
    uint64_t VirtualAddress = (uint64_t)vmm_GetVirtualAddress(lock->Parent->SharedPaging);
    
    if(!vmm_GetFlags(lock->Parent->SharedPaging, (uintptr_t)VirtualAddress, vmm_flag::vmm_Master) || vmm_GetFlags(lock->Parent->SharedPaging, (uintptr_t)VirtualAddress, vmm_flag::vmm_PhysicalStorage) || !vmm_GetFlags(lock->Parent->SharedPaging, (uintptr_t)VirtualAddress, vmm_flag::vmm_Slave)) return KFAIL;
    
    uint64_t PageAddress = lock->Address - (lock->Address % PAGE_SIZE);
    uint64_t Offset = (lock->Address % PAGE_SIZE) / sizeof(uint64_t);
    if(!vmm_GetFlags(lock->Parent->SharedPaging, (uintptr_t)PageAddress, vmm_flag::vmm_Present)) return KFAIL;
    lockreference_t* AccessAddress = (lockreference_t*)(vmm_GetVirtualAddress(vmm_GetPhysical(lock->Parent->SharedPaging, (uintptr_t)PageAddress)));
    if(AccessAddress->LockOffset[Offset] != lock) return KFAIL;
    return KSUCCESS;
}

KResult Keyhole_Get(kthread_t* caller, key_t key, enum DataType type, uint64_t* data, uint64_t* flags){        
    uint64_t Statu = Keyhole_Verify(caller, key, type);
    if(Statu != KSUCCESS) return Statu;
    lock_t* lock = (lock_t*)key;
    *data = lock->Data;
    *flags = lock->Flags;
    return KSUCCESS;
}

KResult Keyhole_Get(kthread_t* caller, key_t key, enum DataType type, lock_t** lock){        
    uint64_t Statu = Keyhole_Verify(caller, key, type);
    if(Statu != KSUCCESS) return Statu;
    *lock = (lock_t*)key;
    return KSUCCESS;
}