#pragma once
#include <main/kernelInit.h>

enum DataType{
    DataTypeAll = 0,
    DataTypeUnknow = 1,
    DataTypeThread = 2,
    DataTypeProcess = 3,
};

namespace Keyhole{
    struct lock_t{
        char Signature0;
        thread_t* Target;
        enum DataType Type;
        char Signature1;
        uint64_t Data;
        char Signature2; 
        bool IsEnable;       
    }__attribute__((packed));

    struct key_t{
        char Signature0;
        process_t* Parent; 
        char Signature1;
        uint64_t Offset;
        char Signature2;
    }__attribute__((packed));

    uint64_t Creat(key_t** key, thread_t* parent, thread_t* target, enum DataType type, uint64_t data);
    uint64_t Duplicate(key_t* key, thread_t* target);
    uint64_t Get(key_t* key, uint64_t* data);
}