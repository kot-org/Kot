#include <poll.h>
#include <errno.h>
#include <fcntl.h>
#include <stddef.h>
#include <limits.h>
#include <signal.h>
#include <lib/log.h>
#include <impl/arch.h>
#include <lib/assert.h>
#include <parameters.h>
#include <kot/syscall.h>
#include <global/heap.h>
#include <global/file.h>
#include <global/time.h>
#include <global/syscall.h>
#include <global/resources.h>
#include <global/elf_loader.h>

#define SYSCALL_RETURN(ctx, return_value) ARCH_CONTEXT_RETURN(ctx) = (arch_context_arg_t)return_value; return;


static void syscall_handler_log(cpu_context_t* ctx){
    char* message = (char*)ARCH_CONTEXT_SYSCALL_ARG0(ctx);
    size_t len = (size_t)ARCH_CONTEXT_SYSCALL_ARG1(ctx);

    if(!vmm_check_memory(vmm_get_current_space(), (memory_range_t){message, len})){
        log_count();
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
    // TODO
    SYSCALL_RETURN(ctx, 0);    
}

static void syscall_handler_mprotect(cpu_context_t* ctx){
    // TODO
    SYSCALL_RETURN(ctx, 0);    
}

static void syscall_handler_exit(cpu_context_t* ctx){
    SYSCALL_RETURN(ctx, -scheduler_exit_process(ARCH_CONTEXT_CURRENT_THREAD(ctx)->process, ctx));
}

static void syscall_handler_thread_exit(cpu_context_t* ctx){
    SYSCALL_RETURN(ctx, -scheduler_exit_thread(ARCH_CONTEXT_CURRENT_THREAD(ctx), ctx));  
}

static void syscall_handler_clock_get(cpu_context_t* ctx){
    int clock = (int)ARCH_CONTEXT_SYSCALL_ARG0(ctx);
    struct timespec* ts = (struct timespec*)ARCH_CONTEXT_SYSCALL_ARG1(ctx);

    // TODO : use clock

    if(vmm_check_memory(vmm_get_current_space(), (memory_range_t){ts, sizeof(struct timespec)})){
        SYSCALL_RETURN(ctx, -EINVAL);
    }

    us_t us = get_current_us();

    ts->tv_sec = TIME_CONVERT_MICROSECOND_TO_SECOND(us);
    ts->tv_nsec = TIME_CONVERT_MICROSECOND_TO_NANOSECOND(TIME_GET_MICROSECOND_UNDER_SECOND(us));

    SYSCALL_RETURN(ctx, 0);     
}

static void syscall_handler_clock_getres(cpu_context_t* ctx){
    log_warning("%s : syscall not implemented\n", __FUNCTION__);
    SYSCALL_RETURN(ctx, -ENOSYS);    
}

static void syscall_handler_sleep(cpu_context_t* ctx){
    struct timespec* ts = (struct timespec*)ARCH_CONTEXT_SYSCALL_ARG0(ctx);

    if(vmm_check_memory(vmm_get_current_space(), (memory_range_t){ts, sizeof(struct timespec)})){
        SYSCALL_RETURN(ctx, -EINVAL);
    }

    us_t us = (us_t)(TIME_CONVERT_SECOND_TO_MICROSECOND(ts->tv_sec) + TIME_CONVERT_NANOSECOND_TO_MICROSECOND(ts->tv_nsec));

    SYSCALL_RETURN(ctx, sleep_us(us));    
}

static void syscall_handler_sigprocmask(cpu_context_t* ctx){
    int how = (int)ARCH_CONTEXT_SYSCALL_ARG0(ctx);
    const sigset_t* set = (const sigset_t*)ARCH_CONTEXT_SYSCALL_ARG1(ctx);
    sigset_t* retrieve = (sigset_t*)ARCH_CONTEXT_SYSCALL_ARG2(ctx);

    if(vmm_check_memory(vmm_get_current_space(), (memory_range_t){(void*)set, sizeof(const sigset_t)})){
        SYSCALL_RETURN(ctx, -EINVAL);
    }

    if(vmm_check_memory(vmm_get_current_space(), (memory_range_t){retrieve, sizeof(sigset_t)})){
        SYSCALL_RETURN(ctx, -EINVAL);
    }

    SYSCALL_RETURN(ctx, 0);    
}

static void syscall_handler_sigaction(cpu_context_t* ctx){
    int signal = (int)ARCH_CONTEXT_SYSCALL_ARG0(ctx);
    const struct sigaction* action = (const struct sigaction*)ARCH_CONTEXT_SYSCALL_ARG1(ctx);
    void* sigreturn = (void*)ARCH_CONTEXT_SYSCALL_ARG2(ctx);
    struct sigaction* old_action = (struct sigaction*)ARCH_CONTEXT_SYSCALL_ARG3(ctx);

    if(signal > SIGNALS_COUNT){
        SYSCALL_RETURN(ctx, -EINVAL);
    }

    signal_handler_t* signal_handler = &ARCH_CONTEXT_CURRENT_THREAD(ctx)->process->signal_handlers[signal - 1];
    
    if(!vmm_check_memory(vmm_get_current_space(), (memory_range_t){old_action, sizeof(struct sigaction)})){
        if(signal_handler->action == signal_handler_action_default){
            *old_action = (struct sigaction){
                {SIG_DFL},
                signal_handler->mask,
                signal_handler->flags,
                NULL
            };
        }else if(signal_handler->action == signal_handler_action_ignore){
            *old_action = (struct sigaction){
                {SIG_IGN},
                signal_handler->mask,
                signal_handler->flags,
                NULL
            };
        }else if(signal_handler->action == signal_handler_action_user){
            *old_action = (struct sigaction){
                {(void (*)(int))(signal_handler->handler)},
                signal_handler->mask,
                signal_handler->flags,
                NULL
            };
        }
    }

    if(!vmm_check_memory(vmm_get_current_space(), (memory_range_t){(void*)action, sizeof(const struct sigaction)})){
        signal_handler->sigreturn = sigreturn;
        signal_handler->mask = action->sa_mask;
        if(action->sa_handler == SIG_DFL){
            signal_handler->action = signal_handler_action_default;
        }else if(action->sa_handler == SIG_IGN){
            signal_handler->action = signal_handler_action_ignore;
        }else{
            signal_handler->handler = action->sa_handler;
            signal_handler->action = signal_handler_action_user;
        }
    }

    SYSCALL_RETURN(ctx, 0);
}

static void syscall_handler_sigrestore(cpu_context_t* ctx){
    SYSCALL_RETURN(ctx, -scheduler_sigrestore(ctx));
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

    SYSCALL_RETURN(ctx, -scheduler_execve_syscall(path, argc, args, envp, ctx));
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

static void syscall_handler_open(cpu_context_t* ctx){
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

    if(flags & O_DIRECTORY){
        kernel_dir_t* dir = d_open(ARCH_CONTEXT_CURRENT_THREAD(ctx)->process->vfs_ctx, path, &error);

        if(error){
            SYSCALL_RETURN(ctx, -error);
        }

        descriptor_t* descriptor = malloc(sizeof(descriptor_t));

        descriptor->type = DESCRIPTOR_TYPE_DIR;
        descriptor->data.dir = dir;

        SYSCALL_RETURN(ctx, add_descriptor(&ARCH_CONTEXT_CURRENT_THREAD(ctx)->process->descriptors_ctx, descriptor));
    }else{
        kernel_file_t* file = f_open(ARCH_CONTEXT_CURRENT_THREAD(ctx)->process->vfs_ctx, path, flags, mode, &error);

        if(error){
            kernel_dir_t* dir = d_open(ARCH_CONTEXT_CURRENT_THREAD(ctx)->process->vfs_ctx, path, &error);

            if(error){
                SYSCALL_RETURN(ctx, -error);
            }

            descriptor_t* descriptor = malloc(sizeof(descriptor_t));

            descriptor->type = DESCRIPTOR_TYPE_DIR;
            descriptor->data.dir = dir;

            SYSCALL_RETURN(ctx, add_descriptor(&ARCH_CONTEXT_CURRENT_THREAD(ctx)->process->descriptors_ctx, descriptor));
        }

        descriptor_t* descriptor = malloc(sizeof(descriptor_t));

        descriptor->type = DESCRIPTOR_TYPE_FILE;
        descriptor->data.file = file;

        SYSCALL_RETURN(ctx, add_descriptor(&ARCH_CONTEXT_CURRENT_THREAD(ctx)->process->descriptors_ctx, descriptor));
    }
}

static void syscall_handler_read(cpu_context_t* ctx){
    int fd = (int)ARCH_CONTEXT_SYSCALL_ARG0(ctx);
    void* buffer = (void*)ARCH_CONTEXT_SYSCALL_ARG1(ctx); 
    size_t size = (size_t)ARCH_CONTEXT_SYSCALL_ARG2(ctx);

    descriptor_t* descriptor = get_descriptor(&ARCH_CONTEXT_CURRENT_THREAD(ctx)->process->descriptors_ctx, fd);

    if(descriptor == NULL){
        SYSCALL_RETURN(ctx, -EBADF);
    }
    
    if(descriptor->type == DESCRIPTOR_TYPE_FILE){
        kernel_file_t* file = descriptor->data.file;

        size_t size_read;

        int error = file->read(buffer, size, &size_read, file);

        if(error){
            SYSCALL_RETURN(ctx, -error);     
        }

        SYSCALL_RETURN(ctx, size_read);
    }else if(descriptor->type == DESCRIPTOR_TYPE_SOCKET){
        kernel_socket_t* socket = descriptor->data.socket;

        size_t size_read;

        int error = socket->read(buffer, size, &size_read, socket);

        if(error){
            SYSCALL_RETURN(ctx, -error);     
        }

        SYSCALL_RETURN(ctx, size_read);
    }else{
        SYSCALL_RETURN(ctx, -EISDIR);
    }

}

static void syscall_handler_write(cpu_context_t* ctx){
    int fd = (int)ARCH_CONTEXT_SYSCALL_ARG0(ctx);
    void* buffer = (void*)ARCH_CONTEXT_SYSCALL_ARG1(ctx); 
    size_t size = (size_t)ARCH_CONTEXT_SYSCALL_ARG2(ctx);

    descriptor_t* descriptor = get_descriptor(&ARCH_CONTEXT_CURRENT_THREAD(ctx)->process->descriptors_ctx, fd);

    if(descriptor == NULL){
        SYSCALL_RETURN(ctx, -EBADF);
    }
    
    if(descriptor->type == DESCRIPTOR_TYPE_FILE){
        kernel_file_t* file = descriptor->data.file;

        size_t size_write;
        int error = file->write(buffer, size, &size_write, file);

        if(error){
            SYSCALL_RETURN(ctx, -error);    
        }

        SYSCALL_RETURN(ctx, size_write);
    }else if(descriptor->type == DESCRIPTOR_TYPE_SOCKET){
        kernel_socket_t* socket = descriptor->data.socket;

        size_t size_write;
        int error = socket->write(buffer, size, &size_write, socket);

        if(error){
            SYSCALL_RETURN(ctx, -error);    
        }

        SYSCALL_RETURN(ctx, size_write);
    }else{
        SYSCALL_RETURN(ctx, -EISDIR);
    }

}

static void syscall_handler_seek(cpu_context_t* ctx){
    int fd = (int)ARCH_CONTEXT_SYSCALL_ARG0(ctx);
    off_t offset = (off_t)ARCH_CONTEXT_SYSCALL_ARG1(ctx); 
    int whence = (int)ARCH_CONTEXT_SYSCALL_ARG2(ctx);

    descriptor_t* descriptor = get_descriptor(&ARCH_CONTEXT_CURRENT_THREAD(ctx)->process->descriptors_ctx, fd);

    if(descriptor == NULL){
        SYSCALL_RETURN(ctx, -EBADF);
    }
    
    if(descriptor->type == DESCRIPTOR_TYPE_FILE){
        kernel_file_t* file = descriptor->data.file;

        off_t new_offset;
        int error = file->seek(offset, whence, &new_offset, file);

        if(error){
            SYSCALL_RETURN(ctx, -error);     
        }

        SYSCALL_RETURN(ctx, new_offset);
    }else if(descriptor->type == DESCRIPTOR_TYPE_SOCKET){
        kernel_socket_t* socket = descriptor->data.socket;

        off_t new_offset;
        int error = socket->seek(offset, whence, &new_offset, socket);

        if(error){
            SYSCALL_RETURN(ctx, -error);     
        }

        SYSCALL_RETURN(ctx, new_offset);       
    }else{
        SYSCALL_RETURN(ctx, -EISDIR);
    }

}

static void syscall_handler_close(cpu_context_t* ctx){
    int fd = (int)ARCH_CONTEXT_SYSCALL_ARG0(ctx);
    
    descriptor_t* descriptor = get_descriptor(&ARCH_CONTEXT_CURRENT_THREAD(ctx)->process->descriptors_ctx, fd);

    if(descriptor == NULL){
        SYSCALL_RETURN(ctx, -EBADF);
    }
    
    if(descriptor->type == DESCRIPTOR_TYPE_FILE){
        if(descriptor->is_parent){
            kernel_file_t* file = descriptor->data.file; 

            int error = file->close(file);

            if(error){
                SYSCALL_RETURN(ctx, -error);
            }

            assert(!remove_descriptor(&ARCH_CONTEXT_CURRENT_THREAD(ctx)->process->descriptors_ctx, fd));

            free(descriptor);

            SYSCALL_RETURN(ctx, 0);
        }else{
            assert(!remove_descriptor(&ARCH_CONTEXT_CURRENT_THREAD(ctx)->process->descriptors_ctx, fd));
            SYSCALL_RETURN(ctx, 0);
        }
    }else if(descriptor->type == DESCRIPTOR_TYPE_SOCKET){
        if(descriptor->is_parent){
            kernel_socket_t* socket = descriptor->data.socket; 

            int error = socket->close(socket);

            if(error){
                SYSCALL_RETURN(ctx, -error);
            }

            assert(!remove_descriptor(&ARCH_CONTEXT_CURRENT_THREAD(ctx)->process->descriptors_ctx, fd));

            free(descriptor);

            SYSCALL_RETURN(ctx, 0);
        }else{
            assert(!remove_descriptor(&ARCH_CONTEXT_CURRENT_THREAD(ctx)->process->descriptors_ctx, fd));
            SYSCALL_RETURN(ctx, 0);
        }
    }else{
        SYSCALL_RETURN(ctx, -EISDIR);
    }

}

static void syscall_handler_ioctl(cpu_context_t* ctx){
    int fd = (int)ARCH_CONTEXT_SYSCALL_ARG0(ctx);
    unsigned long request = (unsigned long)ARCH_CONTEXT_SYSCALL_ARG1(ctx); 
    void* arg = (void*)ARCH_CONTEXT_SYSCALL_ARG2(ctx);

    descriptor_t* descriptor = get_descriptor(&ARCH_CONTEXT_CURRENT_THREAD(ctx)->process->descriptors_ctx, fd);

    if(descriptor == NULL){
        SYSCALL_RETURN(ctx, -EBADF);
    }
    
    if(descriptor->type == DESCRIPTOR_TYPE_FILE){
        kernel_file_t* file = descriptor->data.file;

        int ptr_result;
        int error = file->ioctl(request, arg, &ptr_result, file);

        if(error){
            SYSCALL_RETURN(ctx, -error);     
        }

        SYSCALL_RETURN(ctx, ptr_result); 
    }else if(descriptor->type == DESCRIPTOR_TYPE_SOCKET){
        kernel_socket_t* socket = descriptor->data.socket;

        int ptr_result;
        int error = socket->ioctl(request, arg, &ptr_result, socket);

        if(error){
            SYSCALL_RETURN(ctx, -error);     
        }

        SYSCALL_RETURN(ctx, ptr_result); 
    }else{
        SYSCALL_RETURN(ctx, -EISDIR);
    }

}

static void syscall_handler_dir_read_entries(cpu_context_t* ctx){
    int handle = (int)ARCH_CONTEXT_SYSCALL_ARG0(ctx);
    void* buffer = (void*)ARCH_CONTEXT_SYSCALL_ARG1(ctx); 
    size_t max_size = (size_t)ARCH_CONTEXT_SYSCALL_ARG2(ctx);
    size_t size_read = 0;

    if(vmm_check_memory(vmm_get_current_space(), (memory_range_t){buffer, max_size})){
        SYSCALL_RETURN(ctx, -EINVAL);
    }
        
    descriptor_t* descriptor = get_descriptor(&ARCH_CONTEXT_CURRENT_THREAD(ctx)->process->descriptors_ctx, handle);

    if(descriptor == NULL){
        SYSCALL_RETURN(ctx, -EBADF);
    }
    
    if(descriptor->type != DESCRIPTOR_TYPE_DIR){
        SYSCALL_RETURN(ctx, -EISDIR);
    }

    kernel_dir_t* dir = descriptor->data.dir;

    int error = d_get_entries(buffer, max_size, &size_read, dir);
    
    if(error){
        SYSCALL_RETURN(ctx, -error);
    }

    SYSCALL_RETURN(ctx, size_read); 
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
    int flags = (int)ARCH_CONTEXT_SYSCALL_ARG2(ctx);
    struct stat* statbuf = (struct stat*)ARCH_CONTEXT_SYSCALL_ARG3(ctx);

    if(path_len >= PATH_MAX){
        SYSCALL_RETURN(ctx, -EINVAL);
    }

    if(vmm_check_memory(vmm_get_current_space(), (memory_range_t){statbuf, sizeof(struct stat)})){
        SYSCALL_RETURN(ctx, -EINVAL);
    }

    if(vmm_check_memory(vmm_get_current_space(), (memory_range_t){path, path_len + 1})){
        SYSCALL_RETURN(ctx, -EINVAL);
    }

    path[path_len] = '\0';

    int error = vfs_stat(ARCH_CONTEXT_CURRENT_THREAD(ctx)->process->vfs_ctx, path, flags, statbuf);

    if(error){
        SYSCALL_RETURN(ctx, -error);
    }

    SYSCALL_RETURN(ctx, 0);
}

static void syscall_handler_fd_stat(cpu_context_t* ctx){
    int fd = (int)ARCH_CONTEXT_SYSCALL_ARG0(ctx);
    int flags = (int)ARCH_CONTEXT_SYSCALL_ARG1(ctx); 
    struct stat* statbuf = (struct stat*)ARCH_CONTEXT_SYSCALL_ARG2(ctx);

    if(vmm_check_memory(vmm_get_current_space(), (memory_range_t){statbuf, sizeof(struct stat)})){
        SYSCALL_RETURN(ctx, -EINVAL);
    }
        
    descriptor_t* descriptor = get_descriptor(&ARCH_CONTEXT_CURRENT_THREAD(ctx)->process->descriptors_ctx, fd);

    if(descriptor == NULL){
        SYSCALL_RETURN(ctx, -EBADF);
    }
    
    if(descriptor->type == DESCRIPTOR_TYPE_DIR){
        SYSCALL_RETURN(ctx, -descriptor->data.dir->stat(flags, statbuf, descriptor->data.dir));
    }else if(descriptor->type == DESCRIPTOR_TYPE_FILE){
        SYSCALL_RETURN(ctx, -descriptor->data.file->stat(flags, statbuf, descriptor->data.file));
    }else if(descriptor->type == DESCRIPTOR_TYPE_SOCKET){
        SYSCALL_RETURN(ctx, -descriptor->data.socket->stat(flags, statbuf, descriptor->data.socket));
    }
    
    SYSCALL_RETURN(ctx, -EBADF);
}

static void syscall_handler_fcntl(cpu_context_t* ctx){
    log_warning("%s : syscall not implemented\n", __FUNCTION__);
    SYSCALL_RETURN(ctx, 0);    
}

static void syscall_handler_getcwd(cpu_context_t* ctx){
    char* buffer = (char*)ARCH_CONTEXT_SYSCALL_ARG0(ctx);
    size_t size = (size_t)ARCH_CONTEXT_SYSCALL_ARG1(ctx);

    assert(!spinlock_acquire(&ARCH_CONTEXT_CURRENT_THREAD(ctx)->process->vfs_ctx->cwd_lock));
    size_t cwd_size = ARCH_CONTEXT_CURRENT_THREAD(ctx)->process->vfs_ctx->cwd_size + sizeof((char)'/');
    spinlock_release(&ARCH_CONTEXT_CURRENT_THREAD(ctx)->process->vfs_ctx->cwd_lock);

    if(cwd_size >= size){
        SYSCALL_RETURN(ctx, -ERANGE);    
    }

    if(vmm_check_memory(vmm_get_current_space(), (memory_range_t){buffer, cwd_size + 1})){
        SYSCALL_RETURN(ctx, -EINVAL);    
    }

    assert(!spinlock_acquire(&ARCH_CONTEXT_CURRENT_THREAD(ctx)->process->vfs_ctx->cwd_lock));
    ssize_t size_to_copy = cwd_size - sizeof((char)'/') - sizeof((char)'/');
    if(size_to_copy > 0){
        memcpy(buffer + sizeof((char)'/'), ARCH_CONTEXT_CURRENT_THREAD(ctx)->process->vfs_ctx->cwd, size_to_copy);
    }
    spinlock_release(&ARCH_CONTEXT_CURRENT_THREAD(ctx)->process->vfs_ctx->cwd_lock);

    buffer[0] = '/';
    buffer[cwd_size - sizeof((char)'/')] = '\0';
    
    SYSCALL_RETURN(ctx, 0);    
}

static void syscall_handler_chdir(cpu_context_t* ctx){
    char* path = (char*)ARCH_CONTEXT_SYSCALL_ARG0(ctx);
    size_t size = (size_t)ARCH_CONTEXT_SYSCALL_ARG1(ctx);

    if(vmm_check_memory(vmm_get_current_space(), (memory_range_t){path, size + 1})){
        SYSCALL_RETURN(ctx, -EINVAL);
    }

    path[size] = '\0';


    // Check if dir exist

    int error = 0;
    kernel_dir_t* dir = d_open(KERNEL_VFS_CTX, path, &error);

    if(error){
        SYSCALL_RETURN(ctx, -ENOENT);
    }

    d_close(dir);

    assert(!spinlock_acquire(&ARCH_CONTEXT_CURRENT_THREAD(ctx)->process->vfs_ctx->cwd_lock));

    ssize_t size_to_copy = size;

    if(path[0] == '/'){
        path++;
        size_to_copy--;
    }

    if(path[size - 1] == '/'){
        size_to_copy--;
    }

    size_t cwd_size = size_to_copy + sizeof((char)'/'); 
    ARCH_CONTEXT_CURRENT_THREAD(ctx)->process->vfs_ctx->cwd_size = cwd_size;
    ARCH_CONTEXT_CURRENT_THREAD(ctx)->process->vfs_ctx->cwd = malloc(cwd_size + sizeof((char)'\0'));

    if(size_to_copy > 0){
        memcpy(ARCH_CONTEXT_CURRENT_THREAD(ctx)->process->vfs_ctx->cwd, path, size_to_copy);
    }

    ARCH_CONTEXT_CURRENT_THREAD(ctx)->process->vfs_ctx->cwd[cwd_size - 1] = '/';
    ARCH_CONTEXT_CURRENT_THREAD(ctx)->process->vfs_ctx->cwd[cwd_size] = '\0';

    spinlock_release(&ARCH_CONTEXT_CURRENT_THREAD(ctx)->process->vfs_ctx->cwd_lock);

    SYSCALL_RETURN(ctx, 0);
}

static void syscall_handler_socket(cpu_context_t* ctx){
    int family = (int)ARCH_CONTEXT_SYSCALL_ARG0(ctx);
    int type = (int)ARCH_CONTEXT_SYSCALL_ARG1(ctx);
    int protocol = (int)ARCH_CONTEXT_SYSCALL_ARG2(ctx);

    int error;

    kernel_socket_t* socket = s_socket(family, type, protocol, &error);

    if(!error){
        descriptor_t* descriptor = malloc(sizeof(descriptor_t));
        
        descriptor->type = DESCRIPTOR_TYPE_SOCKET;
        descriptor->data.socket = socket;

        SYSCALL_RETURN(ctx, add_descriptor(&ARCH_CONTEXT_CURRENT_THREAD(ctx)->process->descriptors_ctx, descriptor));
    }else{
        SYSCALL_RETURN(ctx, -error);
    }
}

static void syscall_handler_bind(cpu_context_t* ctx){
    int fd = (int)ARCH_CONTEXT_SYSCALL_ARG0(ctx);
    const struct sockaddr* addr_ptr = (const struct sockaddr*)ARCH_CONTEXT_SYSCALL_ARG1(ctx); 
    socklen_t addr_length = (socklen_t)ARCH_CONTEXT_SYSCALL_ARG2(ctx);

    descriptor_t* descriptor = get_descriptor(&ARCH_CONTEXT_CURRENT_THREAD(ctx)->process->descriptors_ctx, fd);

    if(descriptor == NULL){
        SYSCALL_RETURN(ctx, -EBADF);
    }
    
    if(descriptor->type != DESCRIPTOR_TYPE_SOCKET){
        SYSCALL_RETURN(ctx, -ENOTSOCK);
    }

    kernel_socket_t* socket = descriptor->data.socket;

    SYSCALL_RETURN(ctx, -socket->bind(socket, addr_ptr, addr_length)); 
}

static void syscall_handler_connect(cpu_context_t* ctx){
    int fd = (int)ARCH_CONTEXT_SYSCALL_ARG0(ctx);
    const struct sockaddr* addr_ptr = (const struct sockaddr*)ARCH_CONTEXT_SYSCALL_ARG1(ctx); 
    socklen_t addr_length = (socklen_t)ARCH_CONTEXT_SYSCALL_ARG2(ctx);

    descriptor_t* descriptor = get_descriptor(&ARCH_CONTEXT_CURRENT_THREAD(ctx)->process->descriptors_ctx, fd);

    if(descriptor == NULL){
        SYSCALL_RETURN(ctx, -EBADF);
    }
    
    if(descriptor->type != DESCRIPTOR_TYPE_SOCKET){
        SYSCALL_RETURN(ctx, -ENOTSOCK);
    }

    kernel_socket_t* socket = descriptor->data.socket;

    SYSCALL_RETURN(ctx, -socket->connect(socket, addr_ptr, addr_length));   
}

static void syscall_handler_listen(cpu_context_t* ctx){
    int fd = (int)ARCH_CONTEXT_SYSCALL_ARG0(ctx);
    int backlog = (int)ARCH_CONTEXT_SYSCALL_ARG1(ctx); 

    descriptor_t* descriptor = get_descriptor(&ARCH_CONTEXT_CURRENT_THREAD(ctx)->process->descriptors_ctx, fd);

    if(descriptor == NULL){
        SYSCALL_RETURN(ctx, -EBADF);
    }
    
    if(descriptor->type != DESCRIPTOR_TYPE_SOCKET){
        SYSCALL_RETURN(ctx, -ENOTSOCK);
    }

    kernel_socket_t* socket = descriptor->data.socket;

    SYSCALL_RETURN(ctx, -socket->listen(socket, backlog));    
}

static void syscall_handler_accept(cpu_context_t* ctx){
    int fd = (int)ARCH_CONTEXT_SYSCALL_ARG0(ctx);
    struct sockaddr* addr_ptr = (struct sockaddr*)ARCH_CONTEXT_SYSCALL_ARG1(ctx); 
    socklen_t* addr_length = (socklen_t*)ARCH_CONTEXT_SYSCALL_ARG2(ctx);

    descriptor_t* descriptor = get_descriptor(&ARCH_CONTEXT_CURRENT_THREAD(ctx)->process->descriptors_ctx, fd);

    if(descriptor == NULL){
        SYSCALL_RETURN(ctx, -EBADF);
    }
    
    if(descriptor->type != DESCRIPTOR_TYPE_SOCKET){
        SYSCALL_RETURN(ctx, -ENOTSOCK);
    }

    int error;
    kernel_socket_t* socket = descriptor->data.socket;
    
    kernel_socket_t* child_socket = socket->accept(socket, addr_ptr, addr_length, &error);

    if(!error){
        descriptor_t* descriptor = malloc(sizeof(descriptor_t));
        
        descriptor->type = DESCRIPTOR_TYPE_SOCKET;
        descriptor->data.socket = child_socket;

        SYSCALL_RETURN(ctx, add_descriptor(&ARCH_CONTEXT_CURRENT_THREAD(ctx)->process->descriptors_ctx, descriptor));
    }else{
        SYSCALL_RETURN(ctx, -error);
    } 
}

static void syscall_handler_socket_send(cpu_context_t* ctx){
    int fd = (int)ARCH_CONTEXT_SYSCALL_ARG0(ctx);
    const struct msghdr* hdr = (const struct msghdr*)ARCH_CONTEXT_SYSCALL_ARG1(ctx); 
    int flags = (int)ARCH_CONTEXT_SYSCALL_ARG2(ctx); 

    descriptor_t* descriptor = get_descriptor(&ARCH_CONTEXT_CURRENT_THREAD(ctx)->process->descriptors_ctx, fd);

    if(descriptor == NULL){
        SYSCALL_RETURN(ctx, -EBADF);
    }
    
    if(descriptor->type != DESCRIPTOR_TYPE_SOCKET){
        SYSCALL_RETURN(ctx, -ENOTSOCK);
    }

    kernel_socket_t* socket = descriptor->data.socket;

    size_t size;
    int error = socket->socket_send(socket, (struct msghdr*)hdr, flags, &size);

    if(error){
        SYSCALL_RETURN(ctx, -error);  
    }

    SYSCALL_RETURN(ctx, size);  
}

static void syscall_handler_socket_recv(cpu_context_t* ctx){
    int fd = (int)ARCH_CONTEXT_SYSCALL_ARG0(ctx);
    const struct msghdr* hdr = (const struct msghdr*)ARCH_CONTEXT_SYSCALL_ARG1(ctx); 
    int flags = (int)ARCH_CONTEXT_SYSCALL_ARG2(ctx); 

    descriptor_t* descriptor = get_descriptor(&ARCH_CONTEXT_CURRENT_THREAD(ctx)->process->descriptors_ctx, fd);

    if(descriptor == NULL){
        SYSCALL_RETURN(ctx, -EBADF);
    }
    
    if(descriptor->type != DESCRIPTOR_TYPE_SOCKET){
        SYSCALL_RETURN(ctx, -ENOTSOCK);
    }

    kernel_socket_t* socket = descriptor->data.socket;

    size_t size;
    int error = socket->socket_recv(socket, (struct msghdr*)hdr, flags, &size);

    if(error){
        SYSCALL_RETURN(ctx, -error);  
    }

    SYSCALL_RETURN(ctx, size);  
}

static void syscall_handler_socket_pair(cpu_context_t* ctx){
    int family = (int)ARCH_CONTEXT_SYSCALL_ARG0(ctx);
    int type = (int)ARCH_CONTEXT_SYSCALL_ARG1(ctx);
    int protocol = (int)ARCH_CONTEXT_SYSCALL_ARG2(ctx);
    int* fds = (int*)ARCH_CONTEXT_SYSCALL_ARG3(ctx); 

    if(vmm_check_memory(vmm_get_current_space(), (memory_range_t){fds, sizeof(int) * 2})){
        SYSCALL_RETURN(ctx, -EINVAL);
    }

    kernel_socket_t* first_socket; 
    kernel_socket_t* second_socket; 
    int error = s_socket_pair(family, type, protocol, &first_socket, &second_socket);

    if(!error){
        /* First */
        descriptor_t* first_descriptor = malloc(sizeof(descriptor_t));
        
        first_descriptor->type = DESCRIPTOR_TYPE_SOCKET;
        first_descriptor->data.socket = first_socket;

        fds[0] = add_descriptor(&ARCH_CONTEXT_CURRENT_THREAD(ctx)->process->descriptors_ctx, first_descriptor);
        
        /* Second */
        descriptor_t* second_descriptor = malloc(sizeof(descriptor_t));
        
        second_descriptor->type = DESCRIPTOR_TYPE_SOCKET;
        second_descriptor->data.socket = first_socket;
        
        fds[1] = add_descriptor(&ARCH_CONTEXT_CURRENT_THREAD(ctx)->process->descriptors_ctx, second_descriptor);

        SYSCALL_RETURN(ctx, 0);
    }else{
        SYSCALL_RETURN(ctx, -error);
    }
}


static void syscall_handler_ppoll(cpu_context_t* ctx){
    struct pollfd* fds = (struct pollfd*)ARCH_CONTEXT_SYSCALL_ARG0(ctx);
    int nfds = (int)ARCH_CONTEXT_SYSCALL_ARG1(ctx);
    const struct timespec* timeout = (const struct timespec*)ARCH_CONTEXT_SYSCALL_ARG2(ctx);
    const sigset_t* sigmask = (const sigset_t*)ARCH_CONTEXT_SYSCALL_ARG3(ctx);
    int* num_events = (int*)ARCH_CONTEXT_SYSCALL_ARG4(ctx);

    if(vmm_check_memory(vmm_get_current_space(), (memory_range_t){fds, nfds * sizeof(struct pollfd)})){
        SYSCALL_RETURN(ctx, -EFAULT);
    }

    us_t time_end = 0;

    if(!vmm_check_memory(vmm_get_current_space(), (memory_range_t){(void*)timeout, sizeof(const struct timespec)})){
        time_end = kernel_get_current_us() + (us_t)(TIME_CONVERT_SECOND_TO_MICROSECOND(timeout->tv_sec) + TIME_CONVERT_NANOSECOND_TO_MICROSECOND(timeout->tv_nsec));
    }    

    int event_count = 0;    

    do{
        if(event_count > 0){
            break;
        }

        event_count = 0;

        for(int i = 0; i < nfds; i++){
            descriptor_t* descriptor = get_descriptor(&ARCH_CONTEXT_CURRENT_THREAD(ctx)->process->descriptors_ctx, fds[i].fd);

            if(descriptor != NULL){
                if(descriptor->type == DESCRIPTOR_TYPE_SOCKET){
                    kernel_socket_t* socket = descriptor->data.socket;

                    event_count += socket->socket_get_event(socket, fds[i].events, &fds[i].revents);
                }
            }
        }
    }while(time_end > kernel_get_current_us());

    if(!vmm_check_memory(vmm_get_current_space(), (memory_range_t){num_events, sizeof(int)})){
        *num_events = event_count;
    }
    
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
    syscall_handler_sigrestore,
    scheduler_fork_syscall,
    syscall_handler_waitpid,
    syscall_handler_execve,
    syscall_handler_getpid,
    syscall_handler_getppid,
    syscall_handler_kill,
    syscall_handler_open,
    syscall_handler_read,
    syscall_handler_write,
    syscall_handler_seek,
    syscall_handler_close,
    syscall_handler_ioctl,
    syscall_handler_dir_read_entries,
    syscall_handler_dir_remove,
    syscall_handler_unlink_at,
    syscall_handler_rename_at,
    syscall_handler_path_stat,
    syscall_handler_fd_stat,
    syscall_handler_fcntl,
    syscall_handler_getcwd,
    syscall_handler_chdir,
    syscall_handler_socket,
    syscall_handler_bind,
    syscall_handler_connect,
    syscall_handler_listen,
    syscall_handler_accept,
    syscall_handler_socket_send,
    syscall_handler_socket_recv,
    syscall_handler_socket_pair,
    syscall_handler_ppoll
};

void syscall_handler(cpu_context_t* ctx){
    int syscall_selector = ARCH_CONTEXT_SYSCALL_SELECTOR(ctx);

    if(syscall_selector >= SYS_COUNT){
        #ifdef DEBUG_SYSCALL
        log_info("Syscall : %d, doesn't exist\n", syscall_selector);
        #endif

        SYSCALL_RETURN(ctx, -EINVAL);
    }
    #ifdef DEBUG_SYSCALL
    us_t syscall_time_start = kernel_get_current_us();
    #endif
 
    handlers[syscall_selector](ctx);

    #ifdef DEBUG_SYSCALL
    us_t syscall_time_end = kernel_get_current_us();

    us_t syscall_time_used = syscall_time_end - syscall_time_start;

    log_info("Syscall : %d, return : %d, in %dus\n", syscall_selector, ARCH_CONTEXT_RETURN(ctx), syscall_time_used);
    #endif

    return;
}