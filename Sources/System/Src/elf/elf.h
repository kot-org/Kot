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
    
    KResult loadElf(uintptr_t buffer, enum kot_Priviledge ring, uint64_t identifier, kot_thread_t* mainthread, char* rootpath, bool isVFS);
    bool Check(struct elf_t* self);

    struct elf_t {
        uintptr_t Buffer;
        Elf64_Ehdr* Header;
        uintptr_t phdrs;
        uintptr_t shdrs;
        Elf64_Shdr* shstr;
        Elf64_Shdr* KotSpecific;
        Elf64_Half KotSpecificIndex;
        Elf64_Sym* KotSpecificSymbol;
    };

}