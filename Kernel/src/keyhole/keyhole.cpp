#include <keyhole/keyhole.h>

namespace Keyhole{
    static uint64_t mutexKeyhole;

    uint64_t Creat(PageTableManager* caller, key_t* key, process_t* Parent, process_t* Target, enum DataType type, uint64_t data){
        if(!CheckAddress((void*)key, sizeof(key))) return KFAIL;

        caller->ChangePaging(Parent->SharedPaging);
        
        Atomic::atomicSpinlock(&mutexKeyhole, 0);
        Atomic::atomicLock(&mutexKeyhole, 0);

        lock_t* Locks = parent->Lock;
        lock_t* Lock = NULL;
        uint64_t Index = parent->LockIndex;
        // search free locker
        uint64_t Address = 0;
        uint64_t LastPage = 0;
        for(uint64_t i = &Locks[parent->LockIndex]; i < parent->LockLimit; i += sizeof(lock_t)){}
        while(true){
            Address = &Locks[Index];
            
            uint64_t Page -= Address % 0x1000;
            if(Page != LastPage){
                if(!parent->SharedPaging->GetFlags((void*)Page, PT_Flag::Present)){
                    parent->SharedPaging->MapMemory((void*)Page, globalAllocator.RequestPage());
                    memset((void*)Page, 0, 0x1000);
                }
            }
            if(!Locks[Index]->IsEnable){
                Lock = &Locks[Index];
                break;
            }

            LastPage = Page;
            Index++;
        }

        parent->LockIndex = Index;

        if(Lock == NULL){
            Atomic::atomicUnlock(&mutexScheduler, 0);
            caller->SharedPaging->RestorePaging();
            return KFAIL;
        }

        uint64_t Page -= ((uint64_t)Lock + sizeof(lock_t)) % 0x1000;
        if(!parent->SharedPaging->GetFlags((void*)Page, PT_Flag::Present)){
            parent->SharedPaging->MapMemory((void*)Page, globalAllocator.RequestPage());
            memset((void*)Page, 0, 0x1000);
        }
        
        // fill data
        Lock->IsEnable = true;
        Lock->Type = type;
        Lock->Target = target;
        Lock->Data = data
        
        // signature
        Lock->Signature0 = 'L';
        Lock->Signature1 = 'O';
        Lock->Signature2 = 'K';
        caller->RestorePaging();

        // setup key data
        key->Parent = parent;
        key->Offset = (uint64_t)Lock;

        key->Signature0 = 'K';
        key->Signature1 = 'E';
        key->Signature2 = 'Y';

        Atomic::atomicUnlock(&mutexScheduler, 0);
        return KSUCCESS;
    }

    uint64_t Duplicate(thread_t* caller, key_t* key, process_t* target){
        lock_t* data = malloc(sizeof(lock_t));
        if(Get(caller, key, data, DataTypeAll) != KSUCCESS) return KFAIL;
        return Creat(caller->Paging, key, data->Parent, target, data->Type, data->Data);
    }

    uint64_t Get(thread_t* caller, key_t* key, lock_t* data, enum DataType type){
        // check key address
        if(!CheckAddress((void*)key, sizeof(key))) return KFAIL;

        // check key signature
        if(key->Signature0 != 'K' || key->Signature1 != 'E' || key->Signature2 != 'Y') return KFAIL;
        
        // check parent
        if(!CheckAddress((void*)key->Parent, sizeof(process_t))) return KFAIL;
        if(!CheckAddress((void*)key->Parent->SharedPaging, sizeof(PageTableManager))) return KFAIL;
        
        uint64_t VirtualAddress = globalPageTableManager[GetCoreID()].GetVirtualAddress(key->Parent->SharedPaging->PML4);
        
        if(!ReturnValue->GetFlags(VirtualAddress, PT_Flag::Custom0) || ReturnValue->GetFlags(VirtualAddress, PT_Flag::Custom1) || !ReturnValue->SetFlags(VirtualAddress, PT_Flag::Custom2)) return KFAIL;
        
        caller->Paging->ChangePaging(key->Parent->Paging);

        lock_t* lock = key->Parent->Locks[key->Offset];

        if(lock->Signature0 != 'L' || lock->Signature1 != 'O' || lock->Signature2 != 'K'){
            caller->SharedPaging->RestorePaging();
            return KFAIL;
        } 

        if(lock->Target != caller->Parent){
            caller->SharedPaging->RestorePaging();
            return KFAIL;
        } 

        if(type != DataTypeAll){
            if(lock->Type != type){
                caller->SharedPaging->RestorePaging();
                return KFAIL;
            }            
        }

        if(data != NULL){
            *data = lock->Data;
        }

        caller->SharedPaging->RestorePaging();
        return KSUCCESS;
    }
}