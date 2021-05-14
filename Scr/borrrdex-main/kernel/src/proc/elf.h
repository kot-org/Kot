#pragma once
#include <cstdint>
#include <cstddef>
#include "fs/vfs.h"

// ELF-64 Object File Format 1.5d2 p. 2
typedef uint64_t Elf64_Addr;
typedef uint64_t Elf64_Off;
typedef uint16_t Elf64_Half;
typedef uint32_t Elf64_Word;
typedef int32_t  Elf64_Sword;
typedef uint64_t Elf64_Xword;
typedef int64_t  Elf64_Sxword;

namespace elf {
    constexpr uint32_t MAGIC        = 0x7f454c46; // ELF Magic as one unit

    // File Types - ELF-64 Object File Format 1.5d2 p. 5
    constexpr Elf64_Half ET_NONE    = 0x0000;
    constexpr Elf64_Half ET_REL     = 0x0001;
    constexpr Elf64_Half ET_EXEC    = 0x0002;
    constexpr Elf64_Half ET_DYN     = 0x0003;
    constexpr Elf64_Half ET_CORE    = 0x0004;
    constexpr Elf64_Half ET_LOOS    = 0xFE00;
    constexpr Elf64_Half ET_HIOS    = 0xFEFF;
    constexpr Elf64_Half ET_LOPROC  = 0xFF00;
    constexpr Elf64_Half ET_HIPROC  = 0xFFFF;

    // ELF Identification - ELF-64 Object File Format 1.5d2 p. 3
    constexpr uint8_t EI_MAG0       = 0;
    constexpr uint8_t EI_MAG1       = 1;
    constexpr uint8_t EI_MAG2       = 2;
    constexpr uint8_t EI_MAG3       = 3;
    constexpr uint8_t EI_CLASS      = 4;
    constexpr uint8_t EI_DATA       = 5;
    constexpr uint8_t EI_VERSION    = 6;
    constexpr uint8_t EI_OSABI      = 7;
    constexpr uint8_t EI_ABIVERSION = 8;
    constexpr uint8_t EI_PAD        = 9;
    constexpr uint8_t EI_NIDENT     = 0x10;

    // Sgement Types - ELF-64 Object File Format 1.5d2 p. 12
    constexpr uint32_t PT_NULL      = 0;
    constexpr uint32_t PT_LOAD      = 1;
    constexpr uint32_t PT_DYNAMIC   = 2;
    constexpr uint32_t PT_INTERP    = 3;
    constexpr uint32_t PT_NOTE      = 4;
    constexpr uint32_t PT_SHLIB     = 5;
    constexpr uint32_t PT_PHDR      = 6;
    constexpr uint32_t PT_LOOS      = 0x60000000;
    constexpr uint32_t PT_HIOS      = 0x6fffffff;
    constexpr uint32_t PT_LOPROC    = 0x70000000;
    constexpr uint32_t PT_HIPROC    = 0x7fffffff;

    // Segment Attributes - ELF-64 Object File Format 1.5d2 p. 13
    constexpr uint32_t PF_X         = 0x1;
    constexpr uint32_t PF_W         = 0x2;
    constexpr uint32_t PF_R         = 0x4;
    constexpr uint32_t PF_MASKOS    = 0x00ff0000;
    constexpr uint32_t PF_MASKPROC  = 0xff000000;

    // Object File Classes - ELF-64 Object File Format 1.5d2 p. 5
    constexpr uint8_t ELFCLASS32    = 1;
    constexpr uint8_t ELFCLASS64    = 2;
}

// ELF File Header - ELF-64 Object File Format 1.5d2 p. 3
typedef struct {
    union {
        unsigned char c[elf::EI_NIDENT];
        uint32_t i;
    } e_ident;

    Elf64_Half e_type;
    Elf64_Half e_machine;
    Elf64_Word e_version;
    Elf64_Addr e_entry;
    Elf64_Off e_phoff;
    Elf64_Off e_shoff;
    Elf64_Word e_flags;
    Elf64_Half e_ehsize;
    Elf64_Half e_phentsize;
    Elf64_Half e_phnum;
    Elf64_Half e_shentsize;
    Elf64_Half e_shnum;
    Elf64_Half e_shstrndx;
} __attribute__((packed)) Elf64_Ehdr;

// Program Header Table Entry - ELF-64 Object File Format 1.5d2 p. 12
typedef struct {
    Elf64_Word p_type;
    Elf64_Word p_flags;
    Elf64_Off p_offset;
    Elf64_Addr p_vaddr;
    Elf64_Addr p_paddr;
    Elf64_Xword p_filesz;
    Elf64_Xword p_memsz;
    Elf64_Xword p_align;    
} __attribute__((packed)) Elf64_Phdr;

typedef struct {
    uint64_t location;
    uint64_t size;
    uint64_t pages;
    uint64_t virt_addr;
} elf_loadable_segment_t;

typedef struct {
    uint64_t entry_point;
    elf_loadable_segment_t* loadables;
    size_t loadables_count;
} elf_info_t;

elf_info_t* elf_info_create(openfile_t file, int* outError);
void elf_info_free(elf_info_t* info);