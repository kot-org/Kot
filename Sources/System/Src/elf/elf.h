#pragma once

#include <elf.h>
#include <string.h>
#include <kot/sys.h>
#include <stdlib.h>
#include <kot/math.h>
#include <kot/utils.h>
#include <kot/authorization.h>
#include <kot/uisd/srvs/storage.h>

#include "../core/main.h"

namespace ELF {
    
    KResult loadElf(void* buffer, kot_process_t proc, enum kot_Priviledge privilege, uint64_t identifier, kot_thread_t* mainthread, char* rootpath, bool isVFS, kot_thread_t VFSHandler);
    bool Check(struct elf_t* self);

    struct elf_t {
        void* Buffer;
        __attribute__((packed)) Elf64_Ehdr* Header;
        void* phdrs;
        void* shdrs;
        __attribute__((packed)) Elf64_Shdr* shstr;
        __attribute__((packed)) Elf64_Shdr* KotSpecific;
        __attribute__((packed)) Elf64_Half KotSpecificIndex;
        __attribute__((packed)) Elf64_Sym* KotSpecificSymbol;
    };

}