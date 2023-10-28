#include <errno.h>
#include <stddef.h>
#include <limits.h>
#include <lib/log.h>
#include <impl/arch.h>
#include <lib/assert.h>
#include <kot/syscall.h>
#include <global/heap.h>
#include <global/file.h>
#include <global/syscall.h>
#include <global/resources.h>
#include <global/elf_loader.h>

#define SYSCALL_RETURN(ctx, return_value) ARCH_CONTEXT_RETURN(ctx) = (arch_context_arg_t)return_value; return;


static void syscall_handler_log(cpu_context_t* ctx){
    char* message = (char*)ARCH_CONTEXT_SYSCALL_ARG0(ctx);
    size_t len = (size_t)ARCH_CONTEXT_SYSCALL_ARG1(ctx);

    if(!vmm_check_memory(vmm_get_current_space(), (memory_range_t){message, len})){
        log_printf("%.*s\n", len, message);

        SYSCALL_RETURN(ctx, 0);
    }else{
        SYSCALL_RETURN(ctx, -EINVAL);
    }
}

static void syscall_handler_arch_prctl(cpu_context_t* ctx){
    int code = (int)ARCH_CONTEXT_SYSCALL_ARG0(ctx);
    void* address = (void*)ARCH_CONTEXT_SYSCALL_ARG1(ctx);

    SYSCALL_RETURN(ctx, -scheduler_arch_prctl_thread(ARCH_CONTEXT_CURRENT_THREAD(ctx), code, address));
}

static void syscall_handler_get_tid(cpu_context_t* ctx){
    SYSCALL_RETURN(ctx, ARCH_CONTEXT_CURRENT_THREAD(ctx)->tid);
}

static void syscall_handler_futex_wait(cpu_context_t* ctx){
    log_warning("%s : syscall not implemented\n", __FUNCTION__);
    SYSCALL_RETURN(ctx, -ENOSYS);    
}

static void syscall_handler_futex_wake(cpu_context_t* ctx){
    log_warning("%s : syscall not implemented\n", __FUNCTION__);
    SYSCALL_RETURN(ctx, -ENOSYS);    
}

static void syscall_handler_mmap(cpu_context_t* ctx){
    void* hint = (void*)ARCH_CONTEXT_SYSCALL_ARG0(ctx);
    size_t size = (size_t)ARCH_CONTEXT_SYSCALL_ARG1(ctx);
    int prot = (int)ARCH_CONTEXT_SYSCALL_ARG2(ctx);
    int flags = (int)ARCH_CONTEXT_SYSCALL_ARG3(ctx);
    int fd = (int)ARCH_CONTEXT_SYSCALL_ARG4(ctx);
    off_t offset = (off_t)ARCH_CONTEXT_SYSCALL_ARG5(ctx);

    if(size == 0){
        log_warning("%s : unsupported size: %u\n", __FUNCTION__, size);
        SYSCALL_RETURN(ctx, -EINVAL);
    }

    int unknown_flags = flags & ~(MAP_ANONYMOUS | MAP_PRIVATE | MAP_FIXED);

    if(unknown_flags){
        log_warning("%s : unsupported flags 0x%x\n", __FUNCTION__, flags);
        SYSCALL_RETURN(ctx, -EINVAL);
    }

    if(!(flags & MAP_PRIVATE)){
        log_warning("%s : unsupported flag MAP_PRIVATE\n", __FUNCTION__);
    }

    if(size > 0x40000000){
        log_warning("%s : unsupported size: %u\n", __FUNCTION__, size);
        SYSCALL_RETURN(ctx, -EINVAL);
    }

    void* base_result = NULL;
    if(mm_allocate_region_vm(ARCH_CONTEXT_CURRENT_THREAD(ctx)->process->memory_handler, hint, size, flags & MAP_FIXED, &base_result)){
        log_warning("%s : region is isn't valid\n", __FUNCTION__);
        SYSCALL_RETURN(ctx, -EINVAL);
    }
    
    size_t size_allocate;
    if(mm_allocate_memory_block(ARCH_CONTEXT_CURRENT_THREAD(ctx)->process->memory_handler, base_result, size, prot, &size_allocate)){
        SYSCALL_RETURN(ctx, -EINVAL);
    }

    if(flags & MAP_ANONYMOUS){
        memset(base_result, 0, size); // initalize memory to 0
    }

    SYSCALL_RETURN(ctx, base_result);
}

static void syscall_handler_munmap(cpu_context_t* ctx){
    log_warning("%s : syscall not implemented\n", __FUNCTION__);
    SYSCALL_RETURN(ctx, -ENOSYS);    
}

static void syscall_handler_mprotect(cpu_context_t* ctx){
    log_warning("%s : syscall not implemented\n", __FUNCTION__);
    SYSCALL_RETURN(ctx, -ENOSYS);    
}

static void syscall_handler_exit(cpu_context_t* ctx){
    SYSCALL_RETURN(ctx, -scheduler_exit_process(ARCH_CONTEXT_CURRENT_THREAD(ctx)->process, ctx));
}

static void syscall_handler_thread_exit(cpu_context_t* ctx){
    SYSCALL_RETURN(ctx, -scheduler_exit_thread(ARCH_CONTEXT_CURRENT_THREAD(ctx), ctx));  
}

static void syscall_handler_clock_get(cpu_context_t* ctx){
    log_warning("%s : syscall not implemented\n", __FUNCTION__);
    SYSCALL_RETURN(ctx, -ENOSYS);    
}

static void syscall_handler_clock_getres(cpu_context_t* ctx){
    log_warning("%s : syscall not implemented\n", __FUNCTION__);
    SYSCALL_RETURN(ctx, -ENOSYS);    
}

static void syscall_handler_sleep(cpu_context_t* ctx){
    log_warning("%s : syscall not implemented\n", __FUNCTION__);
    SYSCALL_RETURN(ctx, -ENOSYS);    
}

static void syscall_handler_sigprocmask(cpu_context_t* ctx){
    log_warning("%s : syscall not implemented\n", __FUNCTION__);
    SYSCALL_RETURN(ctx, -ENOSYS);    
}

static void syscall_handler_sigaction(cpu_context_t* ctx){
    log_warning("%s : syscall not implemented\n", __FUNCTION__);
    SYSCALL_RETURN(ctx, -ENOSYS);    
}

static void syscall_handler_waitpid(cpu_context_t* ctx){
    pid_t pid = (pid_t)ARCH_CONTEXT_SYSCALL_ARG0(ctx);
    int* status = (int*)ARCH_CONTEXT_SYSCALL_ARG1(ctx);
    int flags = (int)ARCH_CONTEXT_SYSCALL_ARG2(ctx);
    struct rusage* ru = (struct rusage*)ARCH_CONTEXT_SYSCALL_ARG3(ctx);
    
    SYSCALL_RETURN(ctx, scheduler_waitpid(pid, status, flags, ru, ctx));    
}

static void syscall_handler_execve(cpu_context_t* ctx){
    char* path = (char*)ARCH_CONTEXT_SYSCALL_ARG0(ctx);
    char** args = (char**)ARCH_CONTEXT_SYSCALL_ARG1(ctx);
    char** envp = (char**)ARCH_CONTEXT_SYSCALL_ARG2(ctx);

    // TODO  : check path, args, envp

    int argc = 0;
    
    while(args[argc] != NULL){
        argc++;
    }

    process_t* process = ARCH_CONTEXT_CURRENT_THREAD(ctx)->process;

    thread_t* old_entry_thread = process->entry_thread;

    process->entry_thread = NULL;

    process->vfs_ctx = vfs_copy_ctx(ARCH_CONTEXT_CURRENT_THREAD(ctx)->process->vfs_ctx);

    int error = load_elf_exec(process, path, argc, args, envp);

    if(error){
        SYSCALL_RETURN(ctx, -error);
    }

    // TODO free the old thread

    context_restore(process->entry_thread->ctx, ctx);
}

static void syscall_handler_getpid(cpu_context_t* ctx){
    SYSCALL_RETURN(ctx, ARCH_CONTEXT_CURRENT_THREAD(ctx)->process->pid); 
}

static void syscall_handler_getppid(cpu_context_t* ctx){
    log_warning("%s : syscall not implemented\n", __FUNCTION__);
    SYSCALL_RETURN(ctx, -ENOSYS);    
}

static void syscall_handler_kill(cpu_context_t* ctx){
    log_warning("%s : syscall not implemented\n", __FUNCTION__);
    SYSCALL_RETURN(ctx, -ENOSYS);    
}

static void syscall_handler_file_open(cpu_context_t* ctx){
    /* Although we asked about the size of the path, we expected there to be "\0" at the end, we will force his writing */
    char* path = (char*)ARCH_CONTEXT_SYSCALL_ARG0(ctx);
    size_t path_len = (size_t)ARCH_CONTEXT_SYSCALL_ARG1(ctx);
    int flags = (int)ARCH_CONTEXT_SYSCALL_ARG2(ctx);
    mode_t mode = (mode_t)ARCH_CONTEXT_SYSCALL_ARG3(ctx);

    if(path_len >= PATH_MAX){
        SYSCALL_RETURN(ctx, -EINVAL);
    }

    if(vmm_check_memory(vmm_get_current_space(), (memory_range_t){path, path_len + 1})){
        SYSCALL_RETURN(ctx, -EINVAL);
    }

    path[path_len] = '\0';

    int error;
    kernel_file_t* file = f_open(ARCH_CONTEXT_CURRENT_THREAD(ctx)->process->vfs_ctx, path, flags, mode, &error);

    if(error){
        SYSCALL_RETURN(ctx, -error);
    }

    descriptor_t* descriptor = malloc(sizeof(descriptor_t));

    descriptor->type = DESCRIPTOR_TYPE_FILE;
    descriptor->data.file = file;

    SYSCALL_RETURN(ctx, add_descriptor(&ARCH_CONTEXT_CURRENT_THREAD(ctx)->process->descriptors_ctx, descriptor));  
}

static void syscall_handler_file_read(cpu_context_t* ctx){
    int fd = (int)ARCH_CONTEXT_SYSCALL_ARG0(ctx);
    void* buffer = (void*)ARCH_CONTEXT_SYSCALL_ARG1(ctx); 
    size_t size = (size_t)ARCH_CONTEXT_SYSCALL_ARG2(ctx);

    descriptor_t* descriptor = get_descriptor(&ARCH_CONTEXT_CURRENT_THREAD(ctx)->process->descriptors_ctx, fd);

    if(descriptor == NULL){
        SYSCALL_RETURN(ctx, -EBADF);
    }
    
    if(descriptor->type != DESCRIPTOR_TYPE_FILE){
        SYSCALL_RETURN(ctx, -EISDIR);
    }

    kernel_file_t* file = descriptor->data.file;

    size_t size_read;

    int error = file->read(buffer, size, &size_read, file);

    if(error){
        SYSCALL_RETURN(ctx, -error);     
    }

    SYSCALL_RETURN(ctx, size_read);
}

static void syscall_handler_file_write(cpu_context_t* ctx){
    int fd = (int)ARCH_CONTEXT_SYSCALL_ARG0(ctx);
    void* buffer = (void*)ARCH_CONTEXT_SYSCALL_ARG1(ctx); 
    size_t size = (size_t)ARCH_CONTEXT_SYSCALL_ARG2(ctx);

    descriptor_t* descriptor = get_descriptor(&ARCH_CONTEXT_CURRENT_THREAD(ctx)->process->descriptors_ctx, fd);

    if(descriptor == NULL){
        SYSCALL_RETURN(ctx, -EBADF);
    }
    
    if(descriptor->type != DESCRIPTOR_TYPE_FILE){
        SYSCALL_RETURN(ctx, -EISDIR);
    }

    kernel_file_t* file = descriptor->data.file;

    size_t size_write;
    int error = file->write(buffer, size, &size_write, file);

    if(error){
        SYSCALL_RETURN(ctx, -error);    
    }

    SYSCALL_RETURN(ctx, size_write);
}

static void syscall_handler_file_seek(cpu_context_t* ctx){
    int fd = (int)ARCH_CONTEXT_SYSCALL_ARG0(ctx);
    off_t offset = (off_t)ARCH_CONTEXT_SYSCALL_ARG1(ctx); 
    int whence = (int)ARCH_CONTEXT_SYSCALL_ARG2(ctx);

    descriptor_t* descriptor = get_descriptor(&ARCH_CONTEXT_CURRENT_THREAD(ctx)->process->descriptors_ctx, fd);

    if(descriptor == NULL){
        SYSCALL_RETURN(ctx, -EBADF);
    }
    
    if(descriptor->type != DESCRIPTOR_TYPE_FILE){
        SYSCALL_RETURN(ctx, -EISDIR);
    }

    kernel_file_t* file = descriptor->data.file;

    off_t new_offset;
    int error = file->seek(offset, whence, &new_offset, file);

    if(error){
        SYSCALL_RETURN(ctx, -error);     
    }

    SYSCALL_RETURN(ctx, new_offset);
}

static void syscall_handler_file_close(cpu_context_t* ctx){
    int fd = (int)ARCH_CONTEXT_SYSCALL_ARG0(ctx);
    
    descriptor_t* descriptor = get_descriptor(&ARCH_CONTEXT_CURRENT_THREAD(ctx)->process->descriptors_ctx, fd);

    if(descriptor == NULL){
        SYSCALL_RETURN(ctx, -EBADF);
    }
    
    if(descriptor->type != DESCRIPTOR_TYPE_FILE){
        SYSCALL_RETURN(ctx, -EISDIR);
    }

    kernel_file_t* file = descriptor->data.file; 

    int error = file->close(file);

    if(error){
        SYSCALL_RETURN(ctx, -error);
    }

    assert(!remove_descriptor(&ARCH_CONTEXT_CURRENT_THREAD(ctx)->process->descriptors_ctx, fd));

    free(descriptor);

    SYSCALL_RETURN(ctx, 0);
}

static void syscall_handler_file_ioctl(cpu_context_t* ctx){
    int fd = (int)ARCH_CONTEXT_SYSCALL_ARG0(ctx);
    unsigned long request = (unsigned long)ARCH_CONTEXT_SYSCALL_ARG1(ctx); 
    void* arg = (void*)ARCH_CONTEXT_SYSCALL_ARG2(ctx);

    descriptor_t* descriptor = get_descriptor(&ARCH_CONTEXT_CURRENT_THREAD(ctx)->process->descriptors_ctx, fd);

    if(descriptor == NULL){
        SYSCALL_RETURN(ctx, -EBADF);
    }
    
    if(descriptor->type != DESCRIPTOR_TYPE_FILE){
        SYSCALL_RETURN(ctx, -EISDIR);
    }

    kernel_file_t* file = descriptor->data.file;

    int ptr_result;
    int error = file->ioctl(request, arg, &ptr_result, file);

    if(error){
        SYSCALL_RETURN(ctx, -error);     
    }

    SYSCALL_RETURN(ctx, ptr_result); 
}

static void syscall_handler_dir_read_entries(cpu_context_t* ctx){
    log_warning("%s : syscall not implemented\n", __FUNCTION__);
    SYSCALL_RETURN(ctx, -ENOSYS);    
}

static void syscall_handler_dir_remove(cpu_context_t* ctx){
    log_warning("%s : syscall not implemented\n", __FUNCTION__);
    SYSCALL_RETURN(ctx, -ENOSYS);    
}

static void syscall_handler_unlink_at(cpu_context_t* ctx){
    log_warning("%s : syscall not implemented\n", __FUNCTION__);
    SYSCALL_RETURN(ctx, -ENOSYS);    
}

static void syscall_handler_rename_at(cpu_context_t* ctx){
    log_warning("%s : syscall not implemented\n", __FUNCTION__);
    SYSCALL_RETURN(ctx, -ENOSYS);    
}

static void syscall_handler_path_stat(cpu_context_t* ctx){
    char* path = (char*)ARCH_CONTEXT_SYSCALL_ARG0(ctx);
    size_t path_len = (size_t)ARCH_CONTEXT_SYSCALL_ARG1(ctx);
    struct stat* stat_buf = (struct stat*)ARCH_CONTEXT_SYSCALL_ARG2(ctx);

    if(path_len >= PATH_MAX){
        SYSCALL_RETURN(ctx, -EINVAL);
    }

    if(vmm_check_memory(vmm_get_current_space(), (memory_range_t){stat_buf, sizeof(struct stat)})){
        SYSCALL_RETURN(ctx, -EINVAL);
    }

    if(vmm_check_memory(vmm_get_current_space(), (memory_range_t){path, path_len + 1})){
        SYSCALL_RETURN(ctx, -EINVAL);
    }

    path[path_len] = '\0';

    // TODO

    SYSCALL_RETURN(ctx, ENOSYS);    
}

static void syscall_handler_fd_stat(cpu_context_t* ctx){
    log_warning("%s : syscall not implemented\n", __FUNCTION__);
    SYSCALL_RETURN(ctx, -ENOSYS);    
}

static void syscall_handler_fcntl(cpu_context_t* ctx){
    log_warning("%s : syscall not implemented\n", __FUNCTION__);
    SYSCALL_RETURN(ctx, -ENOSYS);    
}

static void syscall_handler_getcwd(cpu_context_t* ctx){
    char* buffer = (char*)ARCH_CONTEXT_SYSCALL_ARG0(ctx);
    size_t size = (size_t)ARCH_CONTEXT_SYSCALL_ARG1(ctx);

    size_t cwd_size = ARCH_CONTEXT_CURRENT_THREAD(ctx)->process->vfs_ctx->cwd_size;

    if(cwd_size >= size){
        SYSCALL_RETURN(ctx, -ERANGE);    
    }

    if(vmm_check_memory(vmm_get_current_space(), (memory_range_t){buffer, cwd_size + 1})){
        SYSCALL_RETURN(ctx, -EINVAL);    
    }

    memcpy(buffer, ARCH_CONTEXT_CURRENT_THREAD(ctx)->process->vfs_ctx->cwd, cwd_size);
    buffer[cwd_size] = '\0';
    
    SYSCALL_RETURN(ctx, 0);    
}

static syscall_handler_t handlers[SYS_COUNT] = { 
    syscall_handler_log,
    syscall_handler_arch_prctl,
    syscall_handler_get_tid,
    syscall_handler_futex_wait,
    syscall_handler_futex_wake,
    syscall_handler_mmap,
    syscall_handler_munmap,
    syscall_handler_mprotect,
    syscall_handler_exit,
    syscall_handler_thread_exit,
    syscall_handler_clock_get,
    syscall_handler_clock_getres,
    syscall_handler_sleep,
    syscall_handler_sigprocmask,
    syscall_handler_sigaction,
    scheduler_fork_syscall,
    syscall_handler_waitpid,
    syscall_handler_execve,
    syscall_handler_getpid,
    syscall_handler_getppid,
    syscall_handler_kill,
    syscall_handler_file_open,
    syscall_handler_file_read,
    syscall_handler_file_write,
    syscall_handler_file_seek,
    syscall_handler_file_close,
    syscall_handler_file_ioctl,
    syscall_handler_dir_read_entries,
    syscall_handler_dir_remove,
    syscall_handler_unlink_at,
    syscall_handler_rename_at,
    syscall_handler_path_stat,
    syscall_handler_fd_stat,
    syscall_handler_fcntl,
    syscall_handler_getcwd
};

void syscall_handler(cpu_context_t* ctx){
    if(ARCH_CONTEXT_SYSCALL_SELECTOR(ctx) >= SYS_COUNT){
        SYSCALL_RETURN(ctx, -EINVAL);
    }
 
    handlers[ARCH_CONTEXT_SYSCALL_SELECTOR(ctx)](ctx);

    return;
}