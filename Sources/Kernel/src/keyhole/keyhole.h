#pragma once
#include <core/core.h>
#include <kot/sys.h>

#define FlagFullPermissions 0xff


struct lock_t{
    char Signature0;
    struct process_t* Target;
    enum DataType Type;
    struct process_t* Parent;
    uint64_t Address;
    uint64_t Data;
    uint64_t Flags; // 0 : present ; 1 : can duplicate ; 2 : can change flags ; 3 : can change memory ; 4 : execute ; 5 : can kill/pause ; 
    char Signature1;
    char Signature2;       
}__attribute__((packed));

struct lockreference_t{
    lock_t* LockOffset[(PAGE_SIZE / sizeof(uint64_t))];   
}__attribute__((packed));

uint64_t Keyhole_Creat(key_t* key, struct process_t* parent, struct process_t* target, enum DataType type, uint64_t data, uint64_t flags);
uint64_t Keyhole_Duplicate(struct thread_t* caller, key_t key, struct process_t* target);
uint64_t Keyhole_Verify(struct thread_t* caller, key_t key, enum DataType type);
uint64_t Keyhole_Get(struct thread_t* caller, key_t key, enum DataType type, uint64_t* data, uint64_t* flags);
uint64_t Keyhole_Get(struct thread_t* caller, key_t key, enum DataType type, lock_t** lock);
