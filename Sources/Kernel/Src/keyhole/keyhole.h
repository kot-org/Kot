#pragma once
#include <core/core.h>
#include <kot/sys.h>
#include <kot/keyhole.h>


struct lock_t{
    char Signature0;
    struct kprocess_t* Target;
    enum DataType Type;
    struct kprocess_t* Parent;
    uint64_t Address;
    uint64_t Data;
    uint64_t Flags; // 0 : present ; 1 : can clone ; 2 : can change flags ; 3 : can change memory ; 4 : execute ; 5 : can kill/pause ; 
    char Signature1;
    char Signature2;       
}__attribute__((packed));

struct lockreference_t{
    lock_t* LockOffset[(PAGE_SIZE / sizeof(uint64_t))];   
}__attribute__((packed));

KResult Keyhole_Create(key_t* key, struct kprocess_t* parent, struct kprocess_t* target, enum DataType type, uint64_t data, uint64_t flags);
KResult Keyhole_CloneModify(struct kthread_t* caller, key_t key, key_t* newKey, struct kprocess_t* target, uint64_t flags);
KResult Keyhole_Verify(struct kthread_t* caller, key_t key, enum DataType type);
KResult Keyhole_Get(struct kthread_t* caller, key_t key, enum DataType type, uint64_t* data, uint64_t* flags);
KResult Keyhole_Get(struct kthread_t* caller, key_t key, enum DataType type, lock_t** lock);
