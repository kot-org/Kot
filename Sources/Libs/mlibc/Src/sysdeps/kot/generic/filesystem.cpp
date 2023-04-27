#include <stdio.h>
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


namespace Kot{
    uint64_t LockFDList;
    Kot::file_t* FDList[MAX_OPEN_FILES];
    int FDCount = 0;
    uint64_t LockHandleList;
    Kot::directory_t* HandleList[MAX_OPEN_DIRS];
    int HandleCount = 0;

    int OpenShellFile(const char *pathname){
        file_t* Shell = Kot::fopenmf((char*)pathname, 0, O_CREAT | O_RDWR);

        if(Shell == NULL) return -1;

        Shell->ExternalData |= File_Is_TTY;
        
        FDList[0] = Shell;      // stdin
        FDList[1] = Shell;      // stdout
        FDList[2] = Shell;      // stderr

        FDCount = 3;
        return 0;
    }
}

namespace mlibc{

    int sys_open(const char *pathname, int flags, mode_t mode, int *fd){
        Kot::file_t* File = Kot::fopenmf((char*)pathname, flags, mode);
        atomicAcquire(&Kot::LockFDList, 0);
        if(Kot::FDCount >= MAX_OPEN_FILES){
            fclose(File);
            atomicUnlock(&Kot::LockFDList, 0);
            return -1; // Too many open files
        }
        Kot::FDList[Kot::FDCount++] = File;
        *fd = Kot::FDCount - 1;
        atomicUnlock(&Kot::LockFDList, 0);
        return 0;
    }

    int sys_read(int fd, void *buf, size_t count, ssize_t *bytes_read){
        if(fd >= MAX_OPEN_FILES) return -1;
        atomicAcquire(&Kot::LockFDList, 0);
        Kot::file_t* File = Kot::FDList[fd];
        atomicUnlock(&Kot::LockFDList, 0);
        if(File == NULL) return -1;
        if(Kot::fread((uintptr_t)buf, count, 1, File) == KSUCCESS){
            *bytes_read = count;
            return 0;
        }else{
            return -1;
        }
    }

    int sys_write(int fd, const void *buf, size_t count, ssize_t *bytes_written){
        if(count > SSIZE_MAX) return -1;
        if(fd >= MAX_OPEN_FILES) return -1;
        atomicAcquire(&Kot::LockFDList, 0);
        Kot::file_t* File = Kot::FDList[fd];
        atomicUnlock(&Kot::LockFDList, 0);
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
        atomicAcquire(&Kot::LockFDList, 0);
        Kot::file_t* File = Kot::FDList[fd];
        atomicUnlock(&Kot::LockFDList, 0);
        if(File == NULL) return -1;
        fseek(File, offset, whence);
        *new_offset = File->Position;
        return 0;
    }

    int sys_close(int fd){
        if(fd < MAX_OPEN_FILES){
            atomicAcquire(&Kot::LockFDList, 0);
            if(!Kot::FDList[fd]){
                atomicUnlock(&Kot::LockFDList, 0);
                return -1;
            }
            if(fclose(Kot::FDList[fd]) != KSUCCESS){
                atomicUnlock(&Kot::LockFDList, 0);
                return -1;
            }
            Kot::FDCount--;
            for (int i = fd; i < Kot::FDCount; i++) {
                Kot::FDList[i] = Kot::FDList[i + 1];
            }
            atomicUnlock(&Kot::LockFDList, 0);
            return 0;
        }else if(fd < MAX_OPEN_FILES + MAX_OPEN_DIRS){
            atomicAcquire(&Kot::LockHandleList, 0);
            if(!Kot::HandleList[fd]){
                atomicUnlock(&Kot::LockHandleList, 0);
                return -1;
            }
            if(closedir(Kot::HandleList[fd]) != KSUCCESS){
                atomicUnlock(&Kot::LockHandleList, 0);
                return -1;
            }
            Kot::HandleCount--;
            for (int i = fd; i < Kot::HandleCount; i++) {
                Kot::HandleList[i] = Kot::HandleList[i + 1];
            }
            atomicUnlock(&Kot::LockHandleList, 0);
            return 0;
        }
        return -1;
    }

    int sys_flock(int fd, int options){
        // TODO
        __ensure(!"Not implemented");
    }

    int sys_open_dir(const char *path, int *handle){
        Kot::directory_t* Dir = Kot::opendir((char*)path);
        atomicAcquire(&Kot::LockHandleList, 0);
        if(Kot::HandleCount >= MAX_OPEN_DIRS){
            Kot::closedir(Dir);
            atomicUnlock(&Kot::LockHandleList, 0);
            return -1; // Too many open files
        }
        Kot::HandleList[Kot::HandleCount++] = Dir;
        *handle = (Kot::HandleCount - 1) + MAX_OPEN_FILES; // Add MAX_OPEN_FILES to know if it's file or directory
        atomicUnlock(&Kot::LockHandleList, 0);
        return 0;
    }

    int sys_read_entries(int handle, void *buffer, size_t max_size, size_t *bytes_read){
        // TODO
        __ensure(!"Not implemented");
    }

    int sys_pread(int fd, void *buf, size_t n, off_t off, ssize_t *bytes_read){
        // TODO
        __ensure(!"Not implemented");
    }

    // In contrast to the isatty() library function, the sysdep function uses return value
    // zero (and not one) to indicate that the file is a terminal.
    int sys_isatty(int fd){
        if(fd >= MAX_OPEN_FILES) return -1;
        atomicAcquire(&Kot::LockFDList, 0);
        Kot::file_t* File = Kot::FDList[fd];
        atomicUnlock(&Kot::LockFDList, 0);
        if(File == NULL) return -1;
        return ((File->ExternalData & File_Is_TTY) ? 0 : 1);
    }

    int sys_rmdir(const char *path){
        return (Kot::rmdir((char*)path) != KSUCCESS);
    }

    int sys_unlinkat(int dirfd, const char *path, int flags){
        // TODO
        __ensure(!"Not implemented");
    }

    int sys_rename(const char *path, const char *new_path){
        return (Kot::rename((char*)path, (char*)new_path) != KSUCCESS);
    }

    int sys_renameat(int olddirfd, const char *old_path, int newdirfd, const char *new_path){
        // TODO
        __ensure(!"Not implemented");
    }
}