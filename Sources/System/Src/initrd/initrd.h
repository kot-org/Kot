#pragma once

#include <stdlib.h>
#include <string.h>
#include <kot/sys.h>
#include <kot/types.h>
#include <kot/memory.h>

#define MaxName 36

namespace initrd {

    struct Header {
        char signature[5];
        uint64_t version;
        uint64_t filenumber;
        uint64_t initfile;
    }__attribute__((packed));

    struct InitrdFile {
        char name[MaxName];
        uint64_t size;
    }__attribute__((packed));  
    
    struct Info {
        void* baseAddress;
        size64_t size;
        Header* header;
    }__attribute__((packed));

    void Parse(void* baseAddress, size64_t size);
    InitrdFile* Find(char* fileName);
    InitrdFile* FindInitFile();
    bool Read(InitrdFile* address, void* buffer);
    void* Read(InitrdFile* file);
}
