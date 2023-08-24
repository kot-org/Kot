#include <errno.h>
#include <stdbool.h>
#include <lib/log.h>
#include <impl/vmm.h>
#include <lib/lock.h>
#include <lib/assert.h>
#include <lib/string.h>
#include <lib/memory.h>
#include <global/elf.h>
#include <impl/initrd.h>
#include <global/file.h>
#include <global/ksym.h>
#include <global/heap.h>
#include <global/modules.h>

static void unmap_module(void* module_address){
    // TODO
}

static bool check_elf_signature(struct elf64_ehdr* header){
    return (header->e_ident[EI_MAG0] == ELFMAG0 && header->e_ident[EI_MAG1] == ELFMAG1 && header->e_ident[EI_MAG2] == ELFMAG2 && header->e_ident[EI_MAG3] == ELFMAG3);
}

static spinlock_t load_elf_module_lock;

int load_elf_module(module_metadata_t** metadata, int argc, char* args[]){
    kernel_file_t* file = open(args[0], 0);

    if(file == NULL){
        return ENOENT;
    }

    struct elf64_ehdr header;

    file->read(&header, sizeof(struct elf64_ehdr), file);

    if(!check_elf_signature(&header)){
        log_error("Invalid executable file : %s, bad header signature\n", args[0]);
        file->close(file);
        return EINVAL;
    }

    if(header.e_ident[EI_CLASS] != ELFCLASS64){
        log_error("Invalid executable file : %s, wrong elf class\n", args[0]);
        file->close(file);
        return EINVAL;
    }

    if(header.e_type != ET_REL){
        log_error("Invalid executable file : %s, not relocatable\n", args[0]);
        file->close(file);
        return EINVAL;
    }

    spinlock_acquire(&load_elf_module_lock);

    void* module_address = vmm_get_free_contiguous(file->size);
    file->read(module_address, file->size, file);

    for(elf64_half i = 0; i < header.e_shnum; i++){
        struct elf64_shdr* section_header = (struct elf64_shdr*)(module_address + header.e_shoff + header.e_shentsize * i);
        
        if(section_header->sh_type == SHT_NOBITS){
            section_header->sh_addr = (elf64_addr)vmm_get_free_contiguous(section_header->sh_size);
            memset((void*)section_header->sh_addr, 0, section_header->sh_size);
        }else{
            section_header->sh_addr = (elf64_addr)(module_address + section_header->sh_offset);
            if(section_header->sh_addralign && (section_header->sh_addr % section_header->sh_addralign)){
                log_warning("Executable file : %s, not aligned correctly %p, required alignement : %p\n", args[0], section_header->sh_addr, section_header->sh_addralign);
            }
        }
    }

    module_metadata_t* module_data = NULL; 

    for(elf64_half i = 0; i < header.e_shnum; i++){
		struct elf64_shdr* section_header = (struct elf64_shdr*)(module_address + header.e_shoff + header.e_shentsize * i);
		
        if(section_header->sh_type == SHT_SYMTAB){
            struct elf64_shdr* strtab_hdr = (struct elf64_shdr*)(module_address + header.e_shoff + header.e_shentsize * section_header->sh_link);
            char* sym_names = (char*)strtab_hdr->sh_addr;
            struct elf64_sym* sym_table = (struct elf64_sym*)section_header->sh_addr;

            for(uint64_t y = 0; y < section_header->sh_size / sizeof(struct elf64_sym); y++){
                if(sym_table[y].st_shndx > 0 && sym_table[y].st_shndx < SHN_LOPROC){
                    struct elf64_shdr* sh_hdr = (struct elf64_shdr*)(module_address + header.e_shoff + header.e_shentsize * sym_table[y].st_shndx);
                    sym_table[y].st_value = (elf64_addr)sym_table[y].st_value + (elf64_addr)sh_hdr->sh_addr;
                }else if(sym_table[y].st_shndx == SHN_UNDEF){
                    if(sym_table[y].st_name){
                        sym_table[y].st_value = (elf64_addr)ksym_find((char*)((elf64_addr)sym_names + (elf64_addr)sym_table[y].st_name));
                    }
                }

                if(sym_table[y].st_name){
                    if(!strcmp(sym_names + sym_table[y].st_name, "module_metadata")){
                        module_data = (module_metadata_t*)sym_table[y].st_value;
                    }
                }
            } 
        }
	}
    
    if(module_data == NULL){
        log_error("Executable file : %s, isn't a module\n", args[0]);
        unmap_module(module_address);
        file->close(file);
        return EINVAL;
    }

    for(elf64_half i = 0; i < header.e_shnum; i++){
        struct elf64_shdr* section_header = (struct elf64_shdr*)(module_address + header.e_shoff + header.e_shentsize * i);
        
        if(section_header->sh_type == SHT_RELA){
            struct elf64_rela* table = (struct elf64_rela*)section_header->sh_addr;
            struct elf64_shdr* target_section = (struct elf64_shdr*)(module_address + header.e_shoff + header.e_shentsize * section_header->sh_info);
            struct elf64_shdr* symbol_section = (struct elf64_shdr*)(module_address + header.e_shoff + header.e_shentsize * section_header->sh_link);
            struct elf64_sym* symbol_table = (struct elf64_sym*)symbol_section->sh_addr;

            for(uint64_t y = 0; y < section_header->sh_size / sizeof(struct elf64_rela); y++){
                void* target = (void*)(table[y].r_offset + target_section->sh_addr);
                switch(ELF64_R_TYPE(table[y].r_info)){
                    case R_X86_64_64:{
                        *(uint64_t*)target = (uint64_t)symbol_table[ELF64_R_SYM(table[y].r_info)].st_value + (uint64_t)table[y].r_addend;
                        break;
                    }
                    case R_X86_64_32:{
                        *(uint32_t*)target = (uint32_t)(uint32_t)symbol_table[ELF64_R_SYM(table[y].r_info)].st_value + table[y].r_addend;
                        break;
                    }
                    case R_X86_64_PC32:{
                        *(uint32_t*)target = (uint32_t)symbol_table[ELF64_R_SYM(table[y].r_info)].st_value + (uint32_t)table[y].r_addend - (uint32_t)((uintptr_t)target);
                        break;
                    }
                    default:{
                        log_error("Executable file : %s, relocation %d not supported\n", args[0], ELF64_R_TYPE(table[y].r_info));
                        unmap_module(module_address);
                        spinlock_release(&load_elf_module_lock);
                        file->close(file);
                        return EINVAL;
                    }
                }
            }
        }
    }   

    spinlock_release(&load_elf_module_lock);

    file->close(file);
    
    log_printf("Executable file : %s, is successfully dected as a module named : %s\n", args[0], module_data->name);

    *metadata = module_data;

    return module_data->init(argc, args);
}