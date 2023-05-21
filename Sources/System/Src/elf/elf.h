#pragma once

#include <elf.h>
#include <kot/sys.h>
#include <stdlib.h>
#include <kot/math.h>
#include <kot/utils.h>
#include <string.h>
#include <kot/authorization.h>
#include <stdio.h>

#include "../core/main.h"

namespace ELF {
    
    KResult loadElf(uintptr_t buffer, enum Priviledge ring, uint64_t identifier, thread_t* mainthread, char* rootpath, bool isVFS);
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