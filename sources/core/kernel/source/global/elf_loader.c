#include <errno.h>
#include <stdbool.h>
#include <lib/log.h>
#include <lib/math.h>
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
#include <global/elf_loader.h>

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
    assert(!f_read(&header, sizeof(struct elf64_ehdr), &bytes_read, file));

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

    off_t offset;
    f_seek(0, SEEK_SET, &offset, file);

    assert(!f_read(module_address, file->file_size_initial, &bytes_read, file));

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
    
    log_printf("Executable file : %s, is successfully detected as a module named : %s\n", file_path, module_data->name);

    *metadata = module_data;

    return module_data->init(argc, args);
}

/* exec */
#define DYNAMIC_LINKER_BASE_ADDRESS ((void*)0x40000000)

struct load_elf_exec_segments_info{
    void* at_phdr;
    char* ld_path;
};

static inline void* get_kernel_mapped(void* address, void* user_base, void* kernel_base){
    return (void*)((uintptr_t)address - (uintptr_t)user_base + (uintptr_t)kernel_base);
}

/* This function need to be run with its paging to access to its stack*/
static void* load_elf_exec_load_stack(void* at_entry, void* at_phdr, void* at_phent, void* at_phnum, int argc, char* args[], char* envp[], void* stack, void* stack_user_base, void* stack_kernel_base){
    uintptr_t stack_iteration = (uintptr_t)stack;

    int envc = 0;

    if(envp != NULL){
        while(envp[envc] != NULL){
            envc++;
        }
    }
    
    uintptr_t arg_pointers[argc];
    for(int i = 0; i < argc; i++){
        size_t arg_len = strlen(args[i]);
        stack_iteration -= (uintptr_t)arg_len + 1;
        strncpy(get_kernel_mapped((void*)stack_iteration, stack_user_base, stack_kernel_base), args[i], arg_len);
        arg_pointers[i] = (uintptr_t)stack_iteration;
    }
    
    uintptr_t env_pointers[envc];
    for(int i = 0; i < envc; i++){
        size_t env_len = strlen(envp[i]);
        stack_iteration -= (uintptr_t)env_len + 1;
        strncpy(get_kernel_mapped((void*)stack_iteration, stack_user_base, stack_kernel_base), envp[i], env_len);
        env_pointers[i] = (uintptr_t)stack_iteration;
    }

    /* Align stack to 16 bytes */
    stack_iteration = (uintptr_t)stack_iteration - ((uintptr_t)stack_iteration & 0xf);

    if((argc + envc + 1) & 1){
        stack_iteration--;
    }

    stack_iteration = (uintptr_t)((uintptr_t)stack_iteration - (uintptr_t)sizeof(struct auxv));
    ((struct auxv*)get_kernel_mapped((void*)stack_iteration, stack_user_base, stack_kernel_base))->a_type = AT_NULL;
    ((struct auxv*)get_kernel_mapped((void*)stack_iteration, stack_user_base, stack_kernel_base))->a_val =  0;

    stack_iteration = (uintptr_t)((uintptr_t)stack_iteration - (uintptr_t)sizeof(struct auxv));
    ((struct auxv*)get_kernel_mapped((void*)stack_iteration, stack_user_base, stack_kernel_base))->a_type = AT_ENTRY;
    ((struct auxv*)get_kernel_mapped((void*)stack_iteration, stack_user_base, stack_kernel_base))->a_val = at_entry;

    stack_iteration = (uintptr_t)((uintptr_t)stack_iteration - (uintptr_t)sizeof(struct auxv));
    ((struct auxv*)get_kernel_mapped((void*)stack_iteration, stack_user_base, stack_kernel_base))->a_type = AT_PHDR;
    ((struct auxv*)get_kernel_mapped((void*)stack_iteration, stack_user_base, stack_kernel_base))->a_val =  at_phdr;

    stack_iteration = (uintptr_t)((uintptr_t)stack_iteration - (uintptr_t)sizeof(struct auxv));
    ((struct auxv*)get_kernel_mapped((void*)stack_iteration, stack_user_base, stack_kernel_base))->a_type = AT_PHENT;
    ((struct auxv*)get_kernel_mapped((void*)stack_iteration, stack_user_base, stack_kernel_base))->a_val =  at_phent;

    stack_iteration = (uintptr_t)((uintptr_t)stack_iteration - (uintptr_t)sizeof(struct auxv));
    ((struct auxv*)get_kernel_mapped((void*)stack_iteration, stack_user_base, stack_kernel_base))->a_type = AT_PHNUM;
    ((struct auxv*)get_kernel_mapped((void*)stack_iteration, stack_user_base, stack_kernel_base))->a_val =  at_phnum;

    stack_iteration = (uintptr_t)((uintptr_t)stack_iteration - (uintptr_t)sizeof(uintptr_t));
    *(uintptr_t*)get_kernel_mapped((void*)stack_iteration, stack_user_base, stack_kernel_base) = 0; // NULL

    for(int i = 0; i < envc; i++){
        stack_iteration = (uintptr_t)((uintptr_t)stack_iteration - (uintptr_t)sizeof(uintptr_t));
        *(uintptr_t*)get_kernel_mapped((void*)stack_iteration, stack_user_base, stack_kernel_base) = env_pointers[i];   
    }

    stack_iteration = (uintptr_t)((uintptr_t)stack_iteration - (uintptr_t)sizeof(uintptr_t));
    *(uintptr_t*)get_kernel_mapped((void*)stack_iteration, stack_user_base, stack_kernel_base) = 0; // NULL

    for(int i = 0; i < argc; i++){
        stack_iteration = (uintptr_t)((uintptr_t)stack_iteration - (uintptr_t)sizeof(uintptr_t));
        *(uintptr_t*)get_kernel_mapped((void*)stack_iteration, stack_user_base, stack_kernel_base) = arg_pointers[i];   
    }    

    stack_iteration = (uintptr_t)((uintptr_t)stack_iteration - (uintptr_t)sizeof(uintptr_t));
    *(uintptr_t*)get_kernel_mapped((void*)stack_iteration, stack_user_base, stack_kernel_base) = argc;

    return (void*)stack_iteration;
}

static int load_elf_exec_segments(process_t* process_ctx, struct elf64_ehdr* header, void* buffer, struct load_elf_exec_segments_info* segments_info, void* base){
    for(elf64_half i = 0; i < header->e_phnum; i++){
        struct elf64_phdr* phdr = (struct elf64_phdr*)((uintptr_t)buffer + (uintptr_t)header->e_phoff + ((uintptr_t)i * (uintptr_t)header->e_phentsize));

        if(phdr->p_type == PT_LOAD){
            void* address = (void*)((uintptr_t)phdr->p_vaddr + (uintptr_t)base);
            size_t size = phdr->p_memsz;

            void* address_data_to_copy = (void*)((uintptr_t)buffer + (uintptr_t)phdr->p_offset);
            size_t size_data_to_copy = phdr->p_filesz;

            void* address_to_clear = (void*)((uintptr_t)address + (uintptr_t)size_data_to_copy);
            size_t size_to_clear = size - size_data_to_copy;

            size_t size_allocate;
            void* address_allocate;
            assert(!mm_allocate_region_vm(process_ctx->memory_handler, address, size, true, &address_allocate));
            assert(!mm_allocate_memory_block(process_ctx->memory_handler, address, size, PROT_READ | PROT_WRITE | PROT_EXEC, &size_allocate));
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

int load_elf_exec(process_t* process_ctx, char* file_path, int argc, char* args[], char* envp[]){
    int err = 0;
    kernel_file_t* file = f_open(process_ctx->vfs_ctx, file_path, 0, 0, &err);

    if(err){
        return err;
    }

    return load_elf_exec_with_file(process_ctx, file, argc, args, envp);
}

int load_elf_exec_with_file(process_t* process_ctx, kernel_file_t* file, int argc, char* args[], char* envp[]){
    int err = 0;

    struct elf64_ehdr header;

    size_t bytes_read;
    assert(!f_read(&header, sizeof(struct elf64_ehdr), &bytes_read, file));

    if(!check_elf_signature(&header)){
        log_error("Invalid executable file, bad header signature\n");
        f_close(file);
        return EINVAL;
    }

    if(header.e_ident[EI_CLASS] != ELFCLASS64){
        log_error("Invalid executable file, wrong elf class\n");
        f_close(file);
        return EINVAL;
    }

    if(header.e_type != ET_EXEC){
        log_error("Invalid executable file, not executable\n");
        f_close(file);
        return EINVAL;
    }

    void* entry_point = NULL;

    vmm_space_t vmm_space_to_restore = vmm_get_current_space();

    vmm_space_swap(process_ctx->memory_handler->vmm_space);

    void* buffer = malloc(file->file_size_initial);
        
    off_t offset;
    f_seek(0, SEEK_SET, &offset, file);

    assert(!f_read(buffer, file->file_size_initial, &bytes_read, file));

    struct load_elf_exec_segments_info exec_segments_info = {};
    load_elf_exec_segments(process_ctx, &header, buffer, &exec_segments_info, NULL);

    free(buffer);

    f_close(file);
    
    if(exec_segments_info.ld_path != NULL){
        kernel_file_t* ld_file = f_open(process_ctx->vfs_ctx, exec_segments_info.ld_path, 0, 0, &err);
        if(err){
            log_error("Dynamic linker file not found with the following path : %s\n", exec_segments_info.ld_path);
            
            vmm_space_swap(vmm_space_to_restore);

            return err;
        }

        struct elf64_ehdr ld_header;

        assert(!f_read(&ld_header, sizeof(struct elf64_ehdr), &bytes_read, ld_file));

        if(!check_elf_signature(&ld_header)){
            log_error("Invalid dynamic linker file : %s, bad header signature\n", exec_segments_info.ld_path);
            f_close(ld_file);
            return EINVAL;
        }

        if(ld_header.e_ident[EI_CLASS] != ELFCLASS64){
            log_error("Invalid dynamic linker file : %s, wrong elf class\n", exec_segments_info.ld_path);
            f_close(ld_file);
            return EINVAL;
        }

        if(ld_header.e_type != ET_DYN){
            log_error("Invalid dynamic linker file : %s, not dynamic\n", exec_segments_info.ld_path);
            f_close(ld_file);
            return EINVAL;
        }

        void* ld_buffer = malloc(ld_file->file_size_initial);

        f_seek(0, SEEK_SET, &offset, ld_file);

        assert(!f_read(ld_buffer, ld_file->file_size_initial, &bytes_read, ld_file));

        struct load_elf_exec_segments_info ld_segments_info;
        load_elf_exec_segments(process_ctx, &ld_header, ld_buffer, &ld_segments_info, DYNAMIC_LINKER_BASE_ADDRESS);

        free(ld_buffer);

        f_close(ld_file);

        entry_point = (void*)((uintptr_t)ld_header.e_entry + (uintptr_t)DYNAMIC_LINKER_BASE_ADDRESS);
    }else{
        entry_point = (void*)header.e_entry;
    }
    
    vmm_space_swap(vmm_space_to_restore);

    void* stack_base;
    size_t size_allocate;
    assert(!mm_allocate_region_vm(process_ctx->memory_handler, NULL, PROCESS_STACK_SIZE, false, &stack_base));
    assert(!mm_allocate_memory_block(process_ctx->memory_handler, stack_base, PROCESS_STACK_SIZE, PROT_READ | PROT_WRITE, &size_allocate));

    void* stack_end = (void*)((uintptr_t)stack_base + (uintptr_t)PROCESS_STACK_SIZE);
    
    void* kernel_mapped_stack_base = vmm_get_free_contiguous_take_and_release(PROCESS_STACK_SIZE);
    void* kernel_mapped_stack_end = (void*)((uintptr_t)kernel_mapped_stack_base + (uintptr_t)PROCESS_STACK_SIZE);
    
    /* map stack to the kernel space so we can access to it */
    size_t page_count = DIV_ROUNDUP(PROCESS_STACK_SIZE, PAGE_SIZE);
    for(int i = 0; i < page_count; i++){
        vmm_map(
            vmm_get_current_space(), 
            (memory_range_t){(void*)((uintptr_t)kernel_mapped_stack_base + i * PAGE_SIZE), PAGE_SIZE}, 
            (memory_range_t){vmm_get_physical_address(process_ctx->memory_handler->vmm_space, 
            (void*)((uintptr_t)stack_base + i * PAGE_SIZE)), PAGE_SIZE}, 
            MEMORY_FLAG_READABLE | MEMORY_FLAG_WRITABLE
        );
    }

    void* stack = load_elf_exec_load_stack((void*)header.e_entry, exec_segments_info.at_phdr, (void*)(uintptr_t)header.e_phentsize, (void*)(uintptr_t)header.e_phnum, argc, args, envp, stack_end, stack_base, kernel_mapped_stack_base);
    
    vmm_release_free_contiguous_take_and_release();

    spinlock_acquire(&process_ctx->data_lock);

    assert(process_ctx->entry_thread == NULL);

    process_ctx->entry_thread = scheduler_create_thread(process_ctx, entry_point, stack, stack_base, PROCESS_STACK_SIZE);

    spinlock_release(&process_ctx->data_lock);

    return 0;
}