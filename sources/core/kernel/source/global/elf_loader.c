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
#include <global/scheduler.h>

/* general */
static bool check_elf_signature(struct elf64_ehdr* header){
    return (header->e_ident[EI_MAG0] == ELFMAG0 && header->e_ident[EI_MAG1] == ELFMAG1 && header->e_ident[EI_MAG2] == ELFMAG2 && header->e_ident[EI_MAG3] == ELFMAG3);
}

/* modules */
static void unmap_module(void* module_address){
    // TODO
}

static spinlock_t load_elf_module_lock;

int load_elf_module(module_metadata_t** metadata, int argc, char* args[]){
    char* file_path = args[0];

    int err = 0;
    kernel_file_t* file = f_open(KERNEL_VFS_CTX, file_path, 0, 0, &err);

    if(err){
        return err;
    }

    struct elf64_ehdr header;

    size_t bytes_read;
    f_read(&header, sizeof(struct elf64_ehdr), &bytes_read, file);

    if(!check_elf_signature(&header)){
        log_error("Invalid executable file : %s, bad header signature\n", file_path);
        f_close(file);
        return EINVAL;
    }

    if(header.e_ident[EI_CLASS] != ELFCLASS64){
        log_error("Invalid executable file : %s, wrong elf class\n", file_path);
        f_close(file);
        return EINVAL;
    }

    if(header.e_type != ET_REL){
        log_error("Invalid executable file : %s, not relocatable\n", file_path);
        f_close(file);
        return EINVAL;
    }

    spinlock_acquire(&load_elf_module_lock);

    void* module_address = vmm_get_free_contiguous(file->file_size_initial);
    f_read(module_address, file->file_size_initial, &bytes_read, file);

    for(elf64_half i = 0; i < header.e_shnum; i++){
        struct elf64_shdr* section_header = (struct elf64_shdr*)(module_address + header.e_shoff + header.e_shentsize * i);
        
        if(section_header->sh_type == SHT_NOBITS){
            section_header->sh_addr = (elf64_addr)vmm_get_free_contiguous(section_header->sh_size);
            memset((void*)section_header->sh_addr, 0, section_header->sh_size);
        }else{
            section_header->sh_addr = (elf64_addr)(module_address + section_header->sh_offset);
            if(section_header->sh_addralign && (section_header->sh_addr % section_header->sh_addralign)){
                log_warning("Executable file : %s, not aligned correctly %p, required alignement : %p\n", file_path, section_header->sh_addr, section_header->sh_addralign);
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
        log_error("Executable file : %s, isn't a module\n", file_path);
        unmap_module(module_address);
        f_close(file);
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
                        log_error("Executable file : %s, relocation %d not supported\n", file_path, ELF64_R_TYPE(table[y].r_info));
                        unmap_module(module_address);
                        spinlock_release(&load_elf_module_lock);
                        f_close(file);
                        return EINVAL;
                    }
                }
            }
        }
    }   

    spinlock_release(&load_elf_module_lock);

    f_close(file);
    
    log_printf("Executable file : %s, is successfully dected as a module named : %s\n", file_path, module_data->name);

    *metadata = module_data;

    return module_data->init(argc, args);
}

/* exec */
#define DYNAMIC_LINKER_BASE_ADDRESS ((void*)0x40000000)

struct load_elf_exec_segments_info{
    void* at_phdr;
    char* ld_path;
};

static int load_elf_exec_segments(struct elf64_ehdr* header, void* buffer, struct load_elf_exec_segments_info* segments_info, void* base){
    for(elf64_half i = 0; i < header->e_phnum; i++){
        struct elf64_phdr* phdr = (struct elf64_phdr*)((uintptr_t)buffer + (uintptr_t)header->e_phoff + ((uintptr_t)i * (uintptr_t)header->e_phentsize));

        if(phdr->p_type == PT_LOAD){
            void* address = (void*)((uintptr_t)phdr->p_vaddr + (uintptr_t)base);
            size_t size = phdr->p_memsz;

            void* address_data_to_copy = (void*)((uintptr_t)buffer + (uintptr_t)phdr->p_offset);
            size_t size_data_to_copy = phdr->p_filesz;

            void* address_to_clear = (void*)((uintptr_t)address_data_to_copy + (uintptr_t)size_data_to_copy);
            size_t size_to_clear = size - size_data_to_copy;

            vmm_map_allocate(vmm_get_current_space(), (memory_range_t){address, size}, MEMORY_FLAG_READABLE | MEMORY_FLAG_WRITABLE | MEMORY_FLAG_EXECUTABLE | MEMORY_FLAG_USER);

            memcpy(address, address_data_to_copy, size_data_to_copy);

            memset(address_to_clear, 0, size_to_clear);
        }else if(phdr->p_type == PT_PHDR){
            segments_info->at_phdr = (void*)((uintptr_t)phdr->p_vaddr + (uintptr_t)base);
        }else if(phdr->p_type == PT_INTERP){
            if(segments_info->ld_path == NULL){
                void* address_data_to_copy = (void*)((uintptr_t)buffer + (uintptr_t)phdr->p_offset);
                size_t size_data_to_copy = phdr->p_filesz;

                segments_info->ld_path = malloc(size_data_to_copy + 1); // add 1 for the null char at the end
                strncpy(segments_info->ld_path, address_data_to_copy, size_data_to_copy);
            }
        }
    }

    return 0;
}

int load_elf_exec(process_t* process_ctx, int argc, char* args[]){
    char* file_path = args[0];

    int err = 0;
    kernel_file_t* file = f_open(process_ctx->vfs_ctx, file_path, 0, 0, &err);

    if(err){
        return err;
    }

    struct elf64_ehdr header;

    size_t bytes_read;
    f_read(&header, sizeof(struct elf64_ehdr), &bytes_read, file);

    if(!check_elf_signature(&header)){
        log_error("Invalid executable file : %s, bad header signature\n", file_path);
        f_close(file);
        return EINVAL;
    }

    if(header.e_ident[EI_CLASS] != ELFCLASS64){
        log_error("Invalid executable file : %s, wrong elf class\n", file_path);
        f_close(file);
        return EINVAL;
    }

    if(header.e_type != ET_EXEC){
        log_error("Invalid executable file : %s, not executable\n", file_path);
        f_close(file);
        return EINVAL;
    }

    vmm_space_t vmm_space_to_restore = vmm_get_current_space();

    vmm_space_swap(process_ctx->vmm_space);

    void* buffer = malloc(file->file_size_initial);
    f_read(buffer, file->file_size_initial, &bytes_read, file);

    struct load_elf_exec_segments_info exec_segments_info;
    load_elf_exec_segments(&header, buffer, &exec_segments_info, NULL);

    if(exec_segments_info.ld_path != NULL){
        kernel_file_t* ld_file = f_open(process_ctx->vfs_ctx, exec_segments_info.ld_path, 0, 0, &err);
        if(err){
            log_error("Dynamic linker file not found with the following path : %s\n", exec_segments_info.ld_path);
            
            vmm_space_swap(vmm_space_to_restore);
            free(buffer);

            return err;
        }

        struct elf64_ehdr ld_header;

        f_read(&ld_header, sizeof(struct elf64_ehdr), &bytes_read, ld_file);

        if(!check_elf_signature(&ld_header)){
            log_error("Invalid dynamic linker file : %s, bad header signature\n", exec_segments_info.ld_path);
            f_close(file);
            return EINVAL;
        }

        if(ld_header.e_ident[EI_CLASS] != ELFCLASS64){
            log_error("Invalid dynamic linker file : %s, wrong elf class\n", exec_segments_info.ld_path);
            f_close(file);
            return EINVAL;
        }

        if(header.e_type != ET_DYN){
            log_error("Invalid dynamic linker file : %s, not dynamic\n", exec_segments_info.ld_path);
            f_close(file);
            return EINVAL;
        }

        void* ld_buffer = malloc(ld_file->file_size_initial);
        f_read(ld_buffer, ld_file->file_size_initial, &bytes_read, ld_file);

        struct load_elf_exec_segments_info ld_segments_info;
        load_elf_exec_segments(&ld_header, ld_buffer, &ld_segments_info, DYNAMIC_LINKER_BASE_ADDRESS);
    }

    vmm_space_swap(vmm_space_to_restore);

    free(buffer);

    return 0;
}