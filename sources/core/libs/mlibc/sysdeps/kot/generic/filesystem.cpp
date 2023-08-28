#include <stdio.h>
#include <dirent.h>
#include <string.h>
#include <stdlib.h>
#include <sys/ioctl.h>
#include <kot/syscall.h>
#include <bits/ensure.h>
#include <frg/vector.hpp>
#include <mlibc/debug.hpp>
#include <mlibc/allocator.hpp>
#include <mlibc/all-sysdeps.hpp>
#include <abi-bits/seek-whence.h>

namespace mlibc{
    int sys_open(const char *pathname, int flags, mode_t mode, int *fd){
        // TODO
        __ensure(!"Not implemented");
        return 0;
    }

    int sys_read(int fd, void *buf, size_t count, ssize_t *bytes_read){
        // TODO
        __ensure(!"Not implemented");
        return 0;
    }

    int sys_write(int fd, const void *buf, size_t count, ssize_t *bytes_written){
        // TODO
        __ensure(!"Not implemented");
        return 0;
    }

    int sys_seek(int fd, off_t offset, int whence, off_t *new_offset){
        // TODO
        __ensure(!"Not implemented");
        return 0;
    }

    int sys_close(int fd){
        // TODO
        __ensure(!"Not implemented");
        return 0;
    }

    int sys_flock(int fd, int options){
        // TODO
        __ensure(!"Not implemented");
        return 0;
    }

    int sys_ioctl(int fd, unsigned long request, void* arg, int* result){
        // TODO
        __ensure(!"Not implemented");
        return 0;
    }

    int sys_open_dir(const char *path, int *handle){
        // TODO
        __ensure(!"Not implemented");
        return 0;
    }

    int sys_read_entries(int handle, void *buffer, size_t max_size, size_t *bytes_read){
        // TODO
        __ensure(!"Not implemented");
        return 0;
    }

    int sys_pread(int fd, void *buf, size_t n, off_t off, ssize_t *bytes_read){
        // TODO
        __ensure(!"Not implemented");
        return 0;
    }

    // In contrast to the isatty() library function, the sysdep function uses return value
    // zero (and not one) to indicate that the file is a terminal.
    int sys_isatty(int fd){
        // TODO
        __ensure(!"Not implemented");
        return 0;
    }

    int sys_rmdir(const char *path){
        // TODO
        __ensure(!"Not implemented");
        return 0;
    }

    int sys_unlinkat(int dirfd, const char *path, int flags){
        // TODO
        __ensure(!"Not implemented");
        return 0;
    }

    int sys_rename(const char *path, const char *new_path){
        // TODO
        __ensure(!"Not implemented");
        return 0;
    }

    int sys_renameat(int olddirfd, const char *old_path, int newdirfd, const char *new_path){
        // TODO
        __ensure(!"Not implemented");
        return 0;
    }

    int sys_stat(fsfd_target fsfdt, int fd, const char *path, int flags, struct stat *statbuf){
        // TODO
        __ensure(!"Not implemented");
        return 0;
    }

    int sys_fcntl(int fd, int request, va_list args, int *result_value){
        // TODO
        __ensure(!"Not implemented");
        return 0;
    }
}