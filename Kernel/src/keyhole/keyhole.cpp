#include <keyhole/keyhole.h>

namespace Keyhole{
    static uint64_t mutexKeyhole;

    uint64_t Creat(key_t* key, process_t* parent, process_t* target, enum DataType type, uint64_t data, uint64_t flags){
        if(!CheckAddress((void*)key, sizeof(key))) return KFAIL;
        
        Atomic::atomicSpinlock(&mutexKeyhole, 0);
        Atomic::atomicLock(&mutexKeyhole, 0);

        parent->LockIndex++;

        // alloc lock
        lock_t* Lock = (lock_t*)malloc(sizeof(lock_t));

        // add reference to the parent
        uint64_t Address = (parent->LockIndex * sizeof(uint64_t)) + (uint64_t)parent->Locks;
        uint64_t Page = Address - (Address % 0x1000);
        uint64_t Offset = (Address % 0x1000) / sizeof(uint64_t);

        lockreference_t* AccessAddress = (lockreference_t*)vmm_GetVirtualAddress(vmm_GetPhysical(parent->SharedPaging, (void*)Page));
        
        if(!vmm_GetFlags(parent->SharedPaging, (void*)Page, vmm_flag::vmm_Present)){
            vmm_Map(parent->SharedPaging, (void*)Page, globalAllocator.RequestPage());
            memset((void*)AccessAddress, 0, 0x1000);
        }

        AccessAddress->LockOffset[Offset] = (uint64_t)Lock;
        
        // fill data
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

    uint64_t Duplicate(thread_t* caller, key_t key, key_t* newKey, process_t* target, uint64_t flags){
        lock_t* data;
        if(Get(caller, key, DataTypeUnknow, &data) != KSUCCESS) return KFAIL;
        lock_t* lock = (lock_t*)key;
        if(!(data->Flags & 0b01)) return KFAIL;
        if(flags & 0b1){
            if(!(data->Flags & 0b001)) return KFAIL;
            flags = data->Flags;
        }
        return Creat(newKey, data->Parent, target, data->Type, data->Data, flags);
    }

    uint64_t Verify(thread_t* caller, key_t key, enum DataType type){
        // get lock
        lock_t* lock = (lock_t*)key;

        // check lock
        if(!CheckAddress((void*)lock, sizeof(lock_t))) return KFAIL;

        if(lock->Signature0 != 'L' || lock->Signature1 != 'O' || lock->Signature2 != 'K') return KFAIL;

        if(lock->Target != NULL){
            if(lock->Target != caller->Parent) return KFAIL;
        }

        if(type != DataTypeUnknow){
            if(lock->Type != type) return KFAIL;            
        }

        // check parent
        if(!CheckAddress((void*)lock->Parent, sizeof(process_t))) return KFAIL;
        
        uint64_t VirtualAddress = (uint64_t)vmm_GetVirtualAddress(lock->Parent->SharedPaging);
        
        if(!vmm_GetFlags(lock->Parent->SharedPaging, (void*)VirtualAddress, vmm_flag::vmm_Custom0) || vmm_GetFlags(lock->Parent->SharedPaging, (void*)VirtualAddress, vmm_flag::vmm_Custom1) || !vmm_GetFlags(lock->Parent->SharedPaging, (void*)VirtualAddress, vmm_flag::vmm_Custom2)) return KFAIL;
        
        uint64_t Page = lock->Address - (lock->Address % 0x1000);
        uint64_t Offset = (lock->Address % 0x1000) / sizeof(uint64_t);

        if(!vmm_GetFlags(lock->Parent->SharedPaging, (void*)Page, vmm_flag::vmm_Present)) return KFAIL;

        lockreference_t* AccessAddress = (lockreference_t*)(vmm_GetVirtualAddress(vmm_GetPhysical(lock->Parent->SharedPaging, (void*)Page)));
        if(AccessAddress->LockOffset[Offset] != (uint64_t)lock) return KFAIL;
    }

    uint64_t Get(thread_t* caller, key_t key, enum DataType type, uint64_t* data, uint64_t* flags){        
        uint64_t Statu = Verify(caller, key, type);

        if(Statu != KSUCCESS) return Statu;

        lock_t* lock = (lock_t*)key;

        *data = lock->Data;
        *flags = lock->Flags;

        return KSUCCESS;
    }

    uint64_t Get(thread_t* caller, key_t key, enum DataType type, lock_t** lock){        
        uint64_t Statu = Verify(caller, key, type);

        if(Statu != KSUCCESS) return Statu;

        *lock = (lock_t*)key;

        return KSUCCESS;
    }
}