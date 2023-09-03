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
        auto result = do_syscall(SYS_FILE_OPEN, 0, pathname, strlen(pathname), flags);

        if(result < 0){
            return -result;
        }

        *fd = result;
        return 0;
    }

    int sys_read(int fd, void *buf, size_t count, ssize_t *bytes_read){
        auto result = do_syscall(SYS_FILE_READ, fd, buf, count);

        if(result < 0){
            *bytes_read = 0;
            return -result;
        }

        *bytes_read = result;
        return 0;
    }

    int sys_write(int fd, const void *buf, size_t count, ssize_t *bytes_written){
        auto result = do_syscall(SYS_FILE_WRITE, fd, buf, count);

        if(result < 0){
            return -result;
        }

        *bytes_written = result;
        return 0;
    }

    int sys_seek(int fd, off_t offset, int whence, off_t *new_offset){
        auto result = do_syscall(SYS_FILE_SEEK, fd, offset, whence);

        if(result < 0){
            return -result;
        }

        *new_offset = result;
        return 0;
    }

    int sys_close(int fd){
        auto result = do_syscall(SYS_FILE_CLOSE, fd);

        if(result < 0){
            return -result;
        }

        return 0;
    }

    int sys_flock(int fd, int options){
        // TODO
        __ensure(!"Not implemented");
        return 0;
    }

    int sys_ioctl(int fd, unsigned long request, void* arg, int* ptr_result){
        auto result = do_syscall(SYS_FILE_IOCTL, fd, request, arg);

        if(result < 0){
            return -result;
        }

        if(ptr_result != NULL){
            *ptr_result = result;
        }
        return 0;
    }

    int sys_open_dir(const char *path, int *handle){
        return sys_open(path, O_DIRECTORY, 0, handle);
    }

    int sys_read_entries(int handle, void *buffer, size_t max_size, size_t *bytes_read){
        auto result = do_syscall(SYS_DIR_READ_ENTRIES, handle, buffer, max_size);

        if(result < 0){
            *bytes_read = 0;
            return -result;
        }

        *bytes_read = result;
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
        struct winsize ws;
        int result;

        if(!sys_ioctl(fd, TIOCGWINSZ, &ws, &result)){
            return 0;
        }

        return ENOTTY;
    }

    int sys_rmdir(const char *path){
        auto result = do_syscall(SYS_DIR_REMOVE, AT_FDCWD, path, AT_REMOVEDIR);

        if(result < 0){
            return -result;
        }

        return 0;
    }

    int sys_unlinkat(int dirfd, const char *path, int flags){
        auto result = do_syscall(SYS_UNLINK_AT, AT_FDCWD, path, AT_REMOVEDIR);

        if(result < 0){
            return -result;
        }

        return 0;
    }

    int sys_rename(const char *old_path, const char *new_path){
        return sys_renameat(AT_FDCWD, old_path, AT_FDCWD, new_path);
    }

    int sys_renameat(int old_dirfd, const char *old_path, int new_dirfd, const char *new_path){
        auto result = do_syscall(SYS_RENAME_AT, old_dirfd, old_path, new_dirfd, new_path);

        if(result < 0){
            return -result;
        }

        return 0;
    }

    int sys_stat(fsfd_target fsfdt, int fd, const char *path, int flags, struct stat *statbuf){
        auto result = 0;

        switch(fsfdt){
            case fsfd_target::path:{
                result = do_syscall(SYS_PATH_STAT, path, strlen(path), statbuf);
                break;
            }
            case fsfd_target::fd:{
                result = do_syscall(SYS_FD_STAT, fd, statbuf);
                break;
            }
            default:{
                mlibc::infoLogger() << "mlibc warning: sys_stat: unsupported fsfd target" << frg::endlog;
                return EINVAL;
            }
        }

        if(result < 0){
            return -result;
        }

        return 0;
    }

    int sys_fcntl(int fd, int request, va_list args, int* ptr_result){
        auto result = do_syscall(SYS_FCNTL, fd, request, va_arg(args, uint64_t));

        if(result < 0){
            return -result;
        }

        if(ptr_result != NULL){
            *ptr_result = result;
        }
        return 0;
    }
}