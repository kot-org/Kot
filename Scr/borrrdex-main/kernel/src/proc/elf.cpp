#include "elf.h"
#include "fs/vfs.h"
#include "memory/heap.h"
#include "drivers/x86_64/arch.h"

elf_info_t* elf_info_create(openfile_t file, int* outError) {
    VirtualFilesystemFile vf(file, false);
    Elf64_Ehdr elf_hdr;

    if(vf.read(&elf_hdr, sizeof(Elf64_Ehdr)) != sizeof(Elf64_Ehdr)) {
        *outError = -1;
        return nullptr;
    }

    if(__builtin_bswap32(elf_hdr.e_ident.i) != elf::MAGIC) {
        *outError = -2;
        return nullptr;
    }

    if(elf_hdr.e_type != elf::ET_EXEC) {
        *outError = -3;
        return nullptr;
    }

    if(!(elf_hdr.e_ident.c[elf::EI_CLASS] & elf::ELFCLASS64)) {
        *outError = -4;
        return nullptr;
    }

    if(elf_hdr.e_phnum == 0) {
        *outError = -5;
        return nullptr;
    }

    elf_info_t* elf = (elf_info_t *)kcalloc(1, sizeof(elf_info_t));
    elf->entry_point = elf_hdr.e_entry;
    elf->loadables = (elf_loadable_segment_t *)kmalloc(elf_hdr.e_phnum * sizeof(elf_loadable_segment_t));

    uint64_t current_pos = elf_hdr.e_phoff;
    vf.seek(current_pos);
    Elf64_Phdr program_hdr;
    for(int i = 0; i < elf_hdr.e_phnum; i++) {
        if(vf.read(&program_hdr, sizeof(Elf64_Phdr)) != sizeof(Elf64_Phdr)) {
            *outError = -6;
            elf_info_free(elf);
            return nullptr;
        }

        switch(program_hdr.p_type) {
            case elf::PT_NULL:
            case elf::PT_NOTE:
            case elf::PT_PHDR:
                break;
            case elf::PT_LOAD:
                elf->loadables[elf->loadables_count++] = {
                    .location = program_hdr.p_offset,
                    .size = program_hdr.p_filesz,
                    .pages = (program_hdr.p_memsz + PAGE_SIZE - 1) / PAGE_SIZE,
                    .virt_addr = program_hdr.p_vaddr
                };

                break;
        }

        current_pos += elf_hdr.e_phentsize;
    }

    if(elf->loadables_count > 0) {
        return elf;
    }

    *outError = -7;
    elf_info_free(elf);
    return nullptr;
}

void elf_info_free(elf_info_t* info) {
    kfree(info->loadables);
    kfree(info);
}