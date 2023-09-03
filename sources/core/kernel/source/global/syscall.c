#include <errno.h>
#include <stddef.h>
#include <lib/log.h>
#include <impl/arch.h>
#include <kot/syscall.h>
#include <global/syscall.h>


static void syscall_handler_log(cpu_context_t* ctx){
    char* message = (char*)ARCH_CONTEXT_ARG0(ctx);
    size_t len = (size_t)ARCH_CONTEXT_ARG1(ctx);

    log_printf("%.*s\n", len, message);

    ARCH_CONTEXT_RETURN(ctx) = 0;
}

static void syscall_handler_arch_prctl(cpu_context_t* ctx){
    log_warning("%s : syscall not implemented\n", __FUNCTION__);
    ARCH_CONTEXT_RETURN(ctx) = -ENOSYS;
}

static void syscall_handler_get_tid(cpu_context_t* ctx){
    log_warning("%s : syscall not implemented\n", __FUNCTION__);
    ARCH_CONTEXT_RETURN(ctx) = -ENOSYS;    
}

static void syscall_handler_futex_wait(cpu_context_t* ctx){
    log_warning("%s : syscall not implemented\n", __FUNCTION__);
    ARCH_CONTEXT_RETURN(ctx) = -ENOSYS;    
}

static void syscall_handler_futex_wake(cpu_context_t* ctx){
    log_warning("%s : syscall not implemented\n", __FUNCTION__);
    ARCH_CONTEXT_RETURN(ctx) = -ENOSYS;    
}

static void syscall_handler_mmap(cpu_context_t* ctx){
    log_warning("%s : syscall not implemented\n", __FUNCTION__);
    ARCH_CONTEXT_RETURN(ctx) = -ENOSYS;    
}

static void syscall_handler_munmap(cpu_context_t* ctx){
    log_warning("%s : syscall not implemented\n", __FUNCTION__);
    ARCH_CONTEXT_RETURN(ctx) = -ENOSYS;    
}

static void syscall_handler_mprotect(cpu_context_t* ctx){
    log_warning("%s : syscall not implemented\n", __FUNCTION__);
    ARCH_CONTEXT_RETURN(ctx) = -ENOSYS;    
}

static void syscall_handler_exit(cpu_context_t* ctx){
    log_warning("%s : syscall not implemented\n", __FUNCTION__);
    ARCH_CONTEXT_RETURN(ctx) = -ENOSYS;    
}

static void syscall_handler_thread_exit(cpu_context_t* ctx){
    log_warning("%s : syscall not implemented\n", __FUNCTION__);
    ARCH_CONTEXT_RETURN(ctx) = -ENOSYS;    
}

static void syscall_handler_clock_get(cpu_context_t* ctx){
    log_warning("%s : syscall not implemented\n", __FUNCTION__);
    ARCH_CONTEXT_RETURN(ctx) = -ENOSYS;    
}

static void syscall_handler_clock_getres(cpu_context_t* ctx){
    log_warning("%s : syscall not implemented\n", __FUNCTION__);
    ARCH_CONTEXT_RETURN(ctx) = -ENOSYS;    
}

static void syscall_handler_sleep(cpu_context_t* ctx){
    log_warning("%s : syscall not implemented\n", __FUNCTION__);
    ARCH_CONTEXT_RETURN(ctx) = -ENOSYS;    
}

static void syscall_handler_sigprocmask(cpu_context_t* ctx){
    log_warning("%s : syscall not implemented\n", __FUNCTION__);
    ARCH_CONTEXT_RETURN(ctx) = -ENOSYS;    
}

static void syscall_handler_sigaction(cpu_context_t* ctx){
    log_warning("%s : syscall not implemented\n", __FUNCTION__);
    ARCH_CONTEXT_RETURN(ctx) = -ENOSYS;    
}

static void syscall_handler_fork(cpu_context_t* ctx){
    log_warning("%s : syscall not implemented\n", __FUNCTION__);
    ARCH_CONTEXT_RETURN(ctx) = -ENOSYS;    
}

static void syscall_handler_waitpid(cpu_context_t* ctx){
    log_warning("%s : syscall not implemented\n", __FUNCTION__);
    ARCH_CONTEXT_RETURN(ctx) = -ENOSYS;    
}

static void syscall_handler_execve(cpu_context_t* ctx){
    log_warning("%s : syscall not implemented\n", __FUNCTION__);
    ARCH_CONTEXT_RETURN(ctx) = -ENOSYS;    
}

static void syscall_handler_getpid(cpu_context_t* ctx){
    log_warning("%s : syscall not implemented\n", __FUNCTION__);
    ARCH_CONTEXT_RETURN(ctx) = -ENOSYS;    
}

static void syscall_handler_getppid(cpu_context_t* ctx){
    log_warning("%s : syscall not implemented\n", __FUNCTION__);
    ARCH_CONTEXT_RETURN(ctx) = -ENOSYS;    
}

static void syscall_handler_kill(cpu_context_t* ctx){
    log_warning("%s : syscall not implemented\n", __FUNCTION__);
    ARCH_CONTEXT_RETURN(ctx) = -ENOSYS;    
}

static void syscall_handler_file_open(cpu_context_t* ctx){
    log_warning("%s : syscall not implemented\n", __FUNCTION__);
    ARCH_CONTEXT_RETURN(ctx) = -ENOSYS;    
}

static void syscall_handler_file_read(cpu_context_t* ctx){
    log_warning("%s : syscall not implemented\n", __FUNCTION__);
    ARCH_CONTEXT_RETURN(ctx) = -ENOSYS;    
}

static void syscall_handler_file_write(cpu_context_t* ctx){
    log_warning("%s : syscall not implemented\n", __FUNCTION__);
    ARCH_CONTEXT_RETURN(ctx) = -ENOSYS;    
}

static void syscall_handler_file_seek(cpu_context_t* ctx){
    log_warning("%s : syscall not implemented\n", __FUNCTION__);
    ARCH_CONTEXT_RETURN(ctx) = -ENOSYS;    
}

static void syscall_handler_file_close(cpu_context_t* ctx){
    log_warning("%s : syscall not implemented\n", __FUNCTION__);
    ARCH_CONTEXT_RETURN(ctx) = -ENOSYS;    
}

static void syscall_handler_file_ioctl(cpu_context_t* ctx){
    log_warning("%s : syscall not implemented\n", __FUNCTION__);
    ARCH_CONTEXT_RETURN(ctx) = -ENOSYS;    
}

static void syscall_handler_dir_read_entries(cpu_context_t* ctx){
    log_warning("%s : syscall not implemented\n", __FUNCTION__);
    ARCH_CONTEXT_RETURN(ctx) = -ENOSYS;    
}

static void syscall_handler_dir_remove(cpu_context_t* ctx){
    log_warning("%s : syscall not implemented\n", __FUNCTION__);
    ARCH_CONTEXT_RETURN(ctx) = -ENOSYS;    
}

static void syscall_handler_unlink_at(cpu_context_t* ctx){
    log_warning("%s : syscall not implemented\n", __FUNCTION__);
    ARCH_CONTEXT_RETURN(ctx) = -ENOSYS;    
}

static void syscall_handler_rename_at(cpu_context_t* ctx){
    log_warning("%s : syscall not implemented\n", __FUNCTION__);
    ARCH_CONTEXT_RETURN(ctx) = -ENOSYS;    
}

static void syscall_handler_path_stat(cpu_context_t* ctx){
    log_warning("%s : syscall not implemented\n", __FUNCTION__);
    ARCH_CONTEXT_RETURN(ctx) = -ENOSYS;    
}

static void syscall_handler_fd_stat(cpu_context_t* ctx){
    log_warning("%s : syscall not implemented\n", __FUNCTION__);
    ARCH_CONTEXT_RETURN(ctx) = -ENOSYS;    
}

static void syscall_handler_fcntl(cpu_context_t* ctx){
    log_warning("%s : syscall not implemented\n", __FUNCTION__);
    ARCH_CONTEXT_RETURN(ctx) = -ENOSYS;    
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
    syscall_handler_fork,
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
    syscall_handler_fcntl
};

void syscall_handler(cpu_context_t* ctx){
    if(ARCH_CONTEXT_SYSCALL_SELECTOR(ctx) >= SYS_COUNT){
        ARCH_CONTEXT_RETURN(ctx) = EINVAL;
        return;
    }

    handlers[ARCH_CONTEXT_SYSCALL_SELECTOR(ctx)](ctx);

    return;
}