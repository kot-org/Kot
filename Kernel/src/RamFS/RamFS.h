#pragma once
#include "../main/kernelInit.h"

#define MaxName 36

namespace RamFS{

    struct Header{
        char signature[5];
        uint64_t version;
        uint64_t filenumber;
        uint64_t initfile;
    }__attribute__((packed));

    struct File{
        char name[MaxName];
        uint64_t size;
    }__attribute__((packed));  
    
    struct Info{
        void* baseAddress;
        size_t size;
        Header* header;
    }__attribute__((packed));

    void Parse(void* baseAddress, size_t size);
    File* Find(char* fileName);
    File* FindInitFile();
    bool Read(File* address, void* buffer);
}

