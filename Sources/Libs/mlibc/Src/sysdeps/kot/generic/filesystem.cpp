#include <string.h>
#include <kot/sys.h>
#include <bits/ensure.h>
#include <frg/vector.hpp>
#include <mlibc/debug.hpp>
#include <mlibc/allocator.hpp>
#include <kot/uisd/srvs/time.h>
#include <mlibc/all-sysdeps.hpp>
#include <kot/uisd/srvs/storage.h>

#define MAX_OPEN_FILES 256
#define MAX_OPEN_DIRS 256

namespace mlibc{
    uint64_t LockFDList;
    Kot::file_t* FDList[MAX_OPEN_FILES];
    int FDCount = 0;
    uint64_t LockHandleList;
    Kot::directory_t* HandleList[MAX_OPEN_DIRS];
    int HandleCount = 0;

    int sys_open(const char *pathname, int flags, mode_t mode, int *fd){
        Kot::file_t* File = Kot::fopenmf((char*)pathname, flags, mode);
        atomicAcquire(&LockFDList, 0);
        if(FDCount >= MAX_OPEN_FILES){
            fclose(File);
            atomicUnlock(&LockFDList, 0);
            return -1; // Too many open files
        }
        FDList[FDCount++] = File;
        *fd = FDCount - 1;
        atomicUnlock(&LockFDList, 0);
        return 0;
    }

    int sys_read(int fd, void *buf, size_t count, ssize_t *bytes_read){
        if(fd >= MAX_OPEN_FILES) return -1;
        atomicAcquire(&LockFDList, 0);
        Kot::file_t* File = FDList[fd];
        atomicUnlock(&LockFDList, 0);
        if(File == NULL) return -1;
        if(Kot::fread((uintptr_t)buf, count, 1, File) == KSUCCESS){
            *bytes_read = count;
            return 0;
        }else{
            return -1;
        }
    }

    int sys_write(int fd, const void *buf, size_t count, ssize_t *bytes_written){
        if(fd >= MAX_OPEN_FILES) return -1;
        atomicAcquire(&LockFDList, 0);
        Kot::file_t* File = FDList[fd];
        atomicUnlock(&LockFDList, 0);
        if(File == NULL) return -1;
        if(Kot::fwrite((uintptr_t)buf, count, 1, File) == KSUCCESS){
            *bytes_written = count;
            return 0;
        }else{
            return -1;
        }
    }

    int sys_seek(int fd, off_t offset, int whence, off_t *new_offset){
        if(fd >= MAX_OPEN_FILES) return -1;
        atomicAcquire(&LockFDList, 0);
        Kot::file_t* File = FDList[fd];
        atomicUnlock(&LockFDList, 0);
        if(File == NULL) return -1;
        fseek(File, offset, whence);
        *new_offset = File->Position;
        return 0;
    }

    int sys_close(int fd){
        if(fd < MAX_OPEN_FILES){
            atomicAcquire(&LockFDList, 0);
            if(!FDList[fd]){
                atomicUnlock(&LockFDList, 0);
                return -1;
            }
            if(fclose(FDList[fd]) != KSUCCESS){
                atomicUnlock(&LockFDList, 0);
                return -1;
            }
            FDCount--;
            for (int i = fd; i < FDCount; i++) {
                FDList[i] = FDList[i + 1];
            }
            atomicUnlock(&LockFDList, 0);
            return 0;
        }else if(fd < MAX_OPEN_FILES + MAX_OPEN_DIRS){
            atomicAcquire(&LockHandleList, 0);
            if(!HandleList[fd]){
                atomicUnlock(&LockHandleList, 0);
                return -1;
            }
            if(closedir(HandleList[fd]) != KSUCCESS){
                atomicUnlock(&LockHandleList, 0);
                return -1;
            }
            HandleCount--;
            for (int i = fd; i < HandleCount; i++) {
                HandleList[i] = HandleList[i + 1];
            }
            atomicUnlock(&LockHandleList, 0);
            return 0;
        }
        return -1;
    }

    int sys_flock(int fd, int options){
        __ensure(!"Not implemented");
    }

    int sys_open_dir(const char *path, int *handle){
        Kot::directory_t* Dir = Kot::opendir((char*)path);
        atomicAcquire(&LockHandleList, 0);
        if(HandleCount >= MAX_OPEN_DIRS){
            Kot::closedir(Dir);
            atomicUnlock(&LockHandleList, 0);
            return -1; // Too many open files
        }
        HandleList[HandleCount++] = Dir;
        *handle = (HandleCount - 1) + MAX_OPEN_FILES; // Add MAX_OPEN_FILES to know if it's file or directory
        atomicUnlock(&LockHandleList, 0);
        return 0;
    }

    int sys_read_entries(int handle, void *buffer, size_t max_size, size_t *bytes_read){
        __ensure(!"Not implemented");
    }

    int sys_pread(int fd, void *buf, size_t n, off_t off, ssize_t *bytes_read){
        __ensure(!"Not implemented");
    }

    // In contrast to the isatty() library function, the sysdep function uses return value
    // zero (and not one) to indicate that the file is a terminal.
    int sys_isatty(int fd){
        if(fd >= MAX_OPEN_FILES) return -1;
        atomicAcquire(&LockFDList, 0);
        Kot::file_t* File = FDList[fd];
        atomicUnlock(&LockFDList, 0);
        if(File == NULL) return -1;
        return ((File->ExternalData & File_Is_TTY) ? 0 : 1);
    }

    int sys_rmdir(const char *path){
        return (Kot::rmdir((char*)path) != KSUCCESS);
    }

    int sys_unlinkat(int dirfd, const char *path, int flags){
        __ensure(!"Not implemented");
    }

    int sys_rename(const char *path, const char *new_path){
        return (Kot::rename((char*)path, (char*)new_path) != KSUCCESS);
    }

    int sys_renameat(int olddirfd, const char *old_path, int newdirfd, const char *new_path){
        __ensure(!"Not implemented");
    }
}