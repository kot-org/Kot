#include <stdio.h>
#include <dirent.h>
#include <string.h>
#include <stdlib.h>
#include <kot/sys.h>
#include <bits/ensure.h>
#include <frg/vector.hpp>
#include <mlibc/debug.hpp>
#include <mlibc/allocator.hpp>
#include <kot/uisd/srvs/time.h>
#include <mlibc/all-sysdeps.hpp>
#include <abi-bits/seek-whence.h>
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
        if(!File) return -1;
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
        if(fd >= MAX_OPEN_FILES) return EBADF;
        atomicAcquire(&Kot::LockFDList, 0);
        Kot::file_t* File = Kot::FDList[fd];
        atomicUnlock(&Kot::LockFDList, 0);
        if(File == NULL) return EBADF;

        atomicAcquire(&File->Lock, 0);
        Kot::srv_storage_callback_t* CallbackReadFile = Kot::Srv_Storage_Readfile(File, (uintptr_t)buf, File->Position, count, true);
        if(CallbackReadFile->Status == KSUCCESS){
            File->Position += CallbackReadFile->Size;
        }
        atomicUnlock(&File->Lock, 0);
        if(CallbackReadFile->Status == KSUCCESS){
            *bytes_read = CallbackReadFile->Size;
            free(CallbackReadFile);
            return 0;
        }else{
            *bytes_read = NULL;
            free(CallbackReadFile);
            return -1;
        }
    }

    int sys_write(int fd, const void *buf, size_t count, ssize_t *bytes_written){
        if(count > SSIZE_MAX) return -1;
        if(fd >= MAX_OPEN_FILES) return EBADF;
        atomicAcquire(&Kot::LockFDList, 0);
        Kot::file_t* File = Kot::FDList[fd];
        atomicUnlock(&Kot::LockFDList, 0);
        if(File == NULL) return EBADF;
        if(Kot::fwrite((uintptr_t)buf, count, 1, File) == KSUCCESS){
            *bytes_written = count;
            return 0;
        }else{
            return -1;
        }
    }

    int sys_seek(int fd, off_t offset, int whence, off_t *new_offset){
        if(fd >= MAX_OPEN_FILES) return EBADF;
        atomicAcquire(&Kot::LockFDList, 0);
        Kot::file_t* File = Kot::FDList[fd];
        atomicUnlock(&Kot::LockFDList, 0);
        if(File == NULL) return EBADF;
        atomicAcquire(&File->Lock, 0);
        switch(whence){
            case SEEK_SET:{
                File->Position = offset;
                *new_offset = File->Position;
                atomicUnlock(&File->Lock, 0);
                return 0;
            }
            case SEEK_CUR:{
                File->Position += offset;
                *new_offset = File->Position;
                atomicUnlock(&File->Lock, 0);
                return 0;
            }
            case SEEK_END:{
                Kot::srv_storage_callback_t* CallbackFileSize = Kot::Srv_Storage_Getfilesize(File, true);
                if(CallbackFileSize->Status != KSUCCESS){
                    free(CallbackFileSize);
                    atomicUnlock(&File->Lock, 0);
                    return -1;
                }
                size64_t Size = CallbackFileSize->Data;
                File->Position = Size;
                free(CallbackFileSize);
                *new_offset = File->Position;
                atomicUnlock(&File->Lock, 0);
                return 0;
            }
            default:{
                atomicUnlock(&File->Lock, 0);
                return EINVAL;
            }
        }
    }

    int sys_close(int fd){
        if(fd < MAX_OPEN_FILES){
            atomicAcquire(&Kot::LockFDList, 0);
            if(!Kot::FDList[fd]){
                atomicUnlock(&Kot::LockFDList, 0);
                return EBADF;
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
            if(!Kot::HandleList[fd - MAX_OPEN_FILES]){
                atomicUnlock(&Kot::LockHandleList, 0);
                return EBADF;
            }
            if(closedir(Kot::HandleList[fd - MAX_OPEN_FILES]) != KSUCCESS){
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
        return EBADF;
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
        if(handle >= MAX_OPEN_FILES + MAX_OPEN_DIRS) return EBADF;
        atomicAcquire(&Kot::LockHandleList, 0);
        Kot::directory_t* Dir = Kot::HandleList[handle - MAX_OPEN_FILES];
        atomicUnlock(&Kot::LockHandleList, 0);
        if(Dir == NULL) return EBADF;

        Kot::directory_entry_t* KotDirEntry = Kot::readdir(Dir);

        if(KotDirEntry != NULL){
            dirent* DirEntry = (dirent*)buffer;

            DirEntry->d_ino = KotDirEntry->NextEntryPosition;
            DirEntry->d_off = 0;
            DirEntry->d_reclen = sizeof(dirent);
            DirEntry->d_type = KotDirEntry->IsFile;
            strcpy(DirEntry->d_name, KotDirEntry->Name);

            *bytes_read = sizeof(dirent);
            return 0;
        }else{
            *bytes_read = NULL;
            return 0;            
        }
    }

    int sys_pread(int fd, void *buf, size_t n, off_t off, ssize_t *bytes_read){
        // TODO
        __ensure(!"Not implemented");
    }

    // In contrast to the isatty() library function, the sysdep function uses return value
    // zero (and not one) to indicate that the file is a terminal.
    int sys_isatty(int fd){
        if(fd >= MAX_OPEN_FILES) return EBADF;
        atomicAcquire(&Kot::LockFDList, 0);
        Kot::file_t* File = Kot::FDList[fd];
        atomicUnlock(&Kot::LockFDList, 0);
        if(File == NULL) return EBADF;
        return ((File->ExternalData & File_Is_TTY) ? 0 : ENOTTY);
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

    int sys_stat(fsfd_target fsfdt, int fd, const char *path, int flags, struct stat *statbuf){
        if(fd >= MAX_OPEN_FILES) return EBADF;
        atomicAcquire(&Kot::LockFDList, 0);
        Kot::file_t* File = Kot::FDList[fd];
        atomicUnlock(&Kot::LockFDList, 0);
        if(File == NULL) return EBADF;

        Kot::srv_storage_callback_t* CallbackFileSize = Kot::Srv_Storage_Getfilesize(File, true);
        if(CallbackFileSize->Status != KSUCCESS){
            free(File);
            return -1;
        }
        statbuf->st_size = CallbackFileSize->Data;
        free(CallbackFileSize);
        // TODO
        statbuf->st_dev = 0;
        statbuf->st_ino = 0;
        statbuf->st_mode = 0;
        statbuf->st_nlink = 0;
        statbuf->st_uid = 0;
        statbuf->st_gid = 0;
        statbuf->st_rdev = 0;
        statbuf->st_blksize = 0;
        statbuf->st_blocks = 0;
        free(File);
        return 0;
    }

    int sys_fcntl(int fd, int request, va_list args, int *result_value){
        // TODO
        infoLogger() << "mlibc: sys_fcntl unsupported request (" << request << ")" << frg::endlog;
        return 0;
    }
}