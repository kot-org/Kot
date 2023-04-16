#include <kot/syscall.h>
#include <string.h>
#include <mlibc/debug.hpp>
#include <mlibc/all-sysdeps.hpp>

struct KotSpecificData_t KotSpecificData;

// To configure build : meson setup build --cross-file ci/kot.cross-file -Ddefault_library=static --prefix=/usr --wipe
// To install : cd build and DESTDIR=sysroot ninja all install

namespace mlibc{
    void sys_libc_log(const char *message){
        Syscall_16(KSys_Logs, (uint64_t)message, (uint64_t)strlen(message)) == KSUCCESS;
    }

    [[noreturn]] void sys_libc_panic(){
        
    }

    int sys_tcb_set(void *pointer){
        
    }

    int sys_futex_tid(){
        
    }

    int sys_futex_wait(int *pointer, int expected, const struct timespec *time){
        
    }

    int sys_futex_wake(int *pointer){
        
    }

    int sys_anon_allocate(size_t size, void **pointer){
        
    }

    int sys_anon_free(void *pointer, size_t size){
        
    }

    int sys_stat(fsfd_target fsfdt, int fd, const char *path, int flags, struct stat *statbuf){
        
    }

    int sys_vm_map(void *hint, size_t size, int prot, int flags, int fd, off_t offset, void **window){
        
    }

    int sys_vm_unmap(void *pointer, size_t size){
        
    }

    int sys_vm_protect(void *pointer, size_t size, int prot){
        
    }

    [[noreturn]] void sys_exit(int status){
        
    }

    [[noreturn, gnu::weak]] void sys_thread_exit(){
        
    }
    
    int sys_open(const char *pathname, int flags, mode_t mode, int *fd){
        
    }

    int sys_read(int fd, void *buf, size_t count, ssize_t *bytes_read){
        
    }

    int sys_write(int fd, const void *buf, size_t count, ssize_t *bytes_written){
        
    }

    int sys_seek(int fd, off_t offset, int whence, off_t *new_offset){
        
    }

    int sys_close(int fd){
        
    }

    [[gnu::weak]] int sys_flock(int fd, int options){
        
    }

    [[gnu::weak]] int sys_open_dir(const char *path, int *handle){
        
    }

    [[gnu::weak]] int sys_read_entries(int handle, void *buffer, size_t max_size,
            size_t *bytes_read){
        
    }

    [[gnu::weak]] int sys_pread(int fd, void *buf, size_t n, off_t off, ssize_t *bytes_read){
        
    }

    int sys_clock_get(int clock, time_t *secs, long *nanos){
        
    }

    [[gnu::weak]] int sys_clock_getres(int clock, time_t *secs, long *nanos){
        
    }

    [[gnu::weak]] int sys_sleep(time_t *secs, long *nanos){
        
    }

    // In contrast to the isatty() library function, the sysdep function uses return value
    // zero (and not one) to indicate that the file is a terminal.
    [[gnu::weak]] int sys_isatty(int fd){
        
    }

    [[gnu::weak]] int sys_rmdir(const char *path){
        
    }

    [[gnu::weak]] int sys_unlinkat(int dirfd, const char *path, int flags){
        
    }

    [[gnu::weak]] int sys_rename(const char *path, const char *new_path){
        
    }

    [[gnu::weak]] int sys_renameat(int olddirfd, const char *old_path, int newdirfd, const char *new_path){
        
    }

    [[gnu::weak]] int sys_sigprocmask(int how, const sigset_t *__restrict set,
            sigset_t *__restrict retrieve){
        
    }

    [[gnu::weak]] int sys_sigaction(int, const struct sigaction *__restrict,
            struct sigaction *__restrict){
        
    }

    [[gnu::weak]] int sys_fork(pid_t *child){
        
    }

    [[gnu::weak]] int sys_waitpid(pid_t pid, int *status, int flags, struct rusage *ru, pid_t *ret_pid){
        
    }

    [[gnu::weak]] int sys_execve(const char *path, char *const argv[], char *const envp[]){
        
    }

    [[gnu::weak]] pid_t sys_getpid(){
        
    }

    [[gnu::weak]] int sys_kill(int, int){
        
    }
}