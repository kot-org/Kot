#pragma once

#include <kot/sys.h>
#include <kot/heap.h>
#include <kot/types.h>
#include <kot/memory.h>
#include <kot/cstring.h>

#define MaxName 36

namespace initrd {

    struct Header {
        char signature[5];
        uint64_t version;
        uint64_t filenumber;
        uint64_t initfile;
    }__attribute__((packed));

    struct File {
        char name[MaxName];
        uint64_t size;
    }__attribute__((packed));  
    
    struct Info {
        uintptr_t baseAddress;
        size64_t size;
        Header* header;
    }__attribute__((packed));

    void Parse(uintptr_t baseAddress, size64_t size);
    File* Find(char* fileName);
    File* FindInitFile();
    bool Read(File* address, uintptr_t buffer);
    uintptr_t Read(File* file);
}
