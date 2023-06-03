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

uint64_t kot_LockFDList;
kot_file_t* kot_FDList[MAX_OPEN_FILES];
int kot_FDCount;
uint64_t kot_LockHandleList;
kot_directory_t* kot_HandleList[MAX_OPEN_DIRS];
int kot_HandleCount;

uint64_t FSExecFlags;

int kot_InitFS(uint64_t Flags){
    kot_LockFDList = 0;
    kot_LockHandleList = 0;
    kot_HandleCount = 0;
    kot_FDCount = 3;
    FSExecFlags = Flags;
    return 0;
}

extern "C" int kot_OpenShellFile(const char *pathname){
    kot_file_t* Shell = kot_fopenmf((char*)pathname, 0, O_CREAT | O_RDWR);

    if(Shell == NULL) return -1;

    Shell->ExternalData |= File_Is_TTY;
    
    kot_FDList[0] = Shell;      // stdin
    kot_FDList[1] = Shell;      // stdout
    kot_FDList[2] = Shell;      // stderr

    return 0;
}

namespace mlibc{
    int sys_open(const char *pathname, int flags, mode_t mode, int *fd){
        kot_file_t* File = kot_fopenmf((char*)pathname, flags, mode);
        if(!File){
            *fd = 0;
            return -1; 
        } 
        atomicAcquire(&kot_LockFDList, 0);
        if(kot_FDCount >= MAX_OPEN_FILES){
            kot_fclose(File);
            atomicUnlock(&kot_LockFDList, 0);
            *fd = 0;
            return -1; // Too many open files
        }
        kot_FDList[kot_FDCount++] = File;
        *fd = kot_FDCount - 1;
        atomicUnlock(&kot_LockFDList, 0);
        return 0;
    }

    int sys_read(int fd, void *buf, size_t count, ssize_t *bytes_read){
        if(FSExecFlags & EXEC_FLAGS_SHELL_DISABLED){
            if(fd <= 2){ // Is Shell file 
                *bytes_read = count;
                return 0;
            }
        }
        if(fd >= MAX_OPEN_FILES) return EBADF;
        atomicAcquire(&kot_LockFDList, 0);
        kot_file_t* File = kot_FDList[fd];
        atomicUnlock(&kot_LockFDList, 0);
        if(File == NULL) return EBADF;

        atomicAcquire(&File->Lock, 0);
        kot_srv_storage_callback_t* CallbackReadFile = kot_Srv_Storage_Readfile(File, (void*)buf, File->Position, count, true);
        if(CallbackReadFile->Status == KSUCCESS){
            File->Position += CallbackReadFile->Size;
        }
        atomicUnlock(&File->Lock, 0);
        if(CallbackReadFile->Status == KSUCCESS){
            *bytes_read = CallbackReadFile->Size;
            free(CallbackReadFile);
            return 0;
        }else{
            *bytes_read = 0;
            free(CallbackReadFile);
            return -1;
        }
    }

    int sys_write(int fd, const void *buf, size_t count, ssize_t *bytes_written){
        if(FSExecFlags & EXEC_FLAGS_SHELL_DISABLED){
            if(fd <= 2){ // Is Shell file 
                mlibc::infoLogger() << "mlibc warning: shell is not enabled by the launcher" << frg::endlog;
                *bytes_written = count;
                return 0;
            }
        }
        if(count > SSIZE_MAX) return -1;
        if(fd >= MAX_OPEN_FILES) return EBADF;
        atomicAcquire(&kot_LockFDList, 0);
        kot_file_t* File = kot_FDList[fd];
        atomicUnlock(&kot_LockFDList, 0);
        if(File == NULL) return EBADF;
        atomicAcquire(&File->Lock, 0);
        struct kot_srv_storage_callback_t* CallbackWriteFile = kot_Srv_Storage_Writefile(File, (void*)buf, File->Position, count, File->IsDataEnd, true);
        KResult Status = CallbackWriteFile->Status;
        if(Status == KSUCCESS){
            File->Position += count;
            atomicUnlock(&File->Lock, 0);
            free(CallbackWriteFile);
            *bytes_written = count;
            return 0;
        }else{
            atomicUnlock(&File->Lock, 0);
            free(CallbackWriteFile);
            *bytes_written = 0;
            return -1;
        }
    }

    int sys_seek(int fd, off_t offset, int whence, off_t *new_offset){
        if(FSExecFlags & EXEC_FLAGS_SHELL_DISABLED){
            if(fd <= 2){ // Is Shell file 
                *new_offset = 0;
                return 0;
            }
        }
        if(fd >= MAX_OPEN_FILES) return EBADF;
        atomicAcquire(&kot_LockFDList, 0);
        kot_file_t* File = kot_FDList[fd];
        atomicUnlock(&kot_LockFDList, 0);
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
                kot_srv_storage_callback_t* CallbackFileSize = kot_Srv_Storage_Getfilesize(File, true);
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

    int kot_Sys_Close(int fd){
        if(FSExecFlags & EXEC_FLAGS_SHELL_DISABLED){
            if(fd <= 2){ // Is Shell file 
                return 0;
            }
        }
        if(fd < MAX_OPEN_FILES){
            atomicAcquire(&kot_LockFDList, 0);
            if(!kot_FDList[fd]){
                atomicUnlock(&kot_LockFDList, 0);
                return EBADF;
            }
            if(kot_fclose(kot_FDList[fd]) != KSUCCESS){
                atomicUnlock(&kot_LockFDList, 0);
                return -1;
            }
            kot_FDCount--;
            for (int i = fd; i < kot_FDCount; i++) {
                kot_FDList[i] = kot_FDList[i + 1];
            }
            atomicUnlock(&kot_LockFDList, 0);
            return 0;
        }else if(fd < MAX_OPEN_FILES + MAX_OPEN_DIRS){
            atomicAcquire(&kot_LockHandleList, 0);
            if(!kot_HandleList[fd - MAX_OPEN_FILES]){
                atomicUnlock(&kot_LockHandleList, 0);
                return EBADF;
            }
            if(kot_closedir(kot_HandleList[fd - MAX_OPEN_FILES]) != KSUCCESS){
                atomicUnlock(&kot_LockHandleList, 0);
                return -1;
            }
            kot_HandleCount--;
            for (int i = fd; i < kot_HandleCount; i++) {
                kot_HandleList[i] = kot_HandleList[i + 1];
            }
            atomicUnlock(&kot_LockHandleList, 0);
            return 0;
        }
        return EBADF;
    }

    int sys_flock(int fd, int options){
        // TODO
        __ensure(!"Not implemented");
    }

    int sys_open_dir(const char *path, int *handle){
        kot_directory_t* Dir = kot_opendir((char*)path);
        atomicAcquire(&kot_LockHandleList, 0);
        if(kot_HandleCount >= MAX_OPEN_DIRS){
            kot_closedir(Dir);
            atomicUnlock(&kot_LockHandleList, 0);
            return -1; // Too many open files
        }
        kot_HandleList[kot_HandleCount++] = Dir;
        *handle = (kot_HandleCount - 1) + MAX_OPEN_FILES; // Add MAX_OPEN_FILES to know if it's file or directory
        atomicUnlock(&kot_LockHandleList, 0);
        return 0;
    }

    int sys_read_entries(int handle, void *buffer, size_t max_size, size_t *bytes_read){
        if(handle >= MAX_OPEN_FILES + MAX_OPEN_DIRS) return EBADF;
        atomicAcquire(&kot_LockHandleList, 0);
        kot_directory_t* Dir = kot_HandleList[handle - MAX_OPEN_FILES];
        atomicUnlock(&kot_LockHandleList, 0);
        if(Dir == NULL) return EBADF;

        kot_directory_entry_t* KotDirEntry = kot_readdir(Dir);

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
        if(FSExecFlags & EXEC_FLAGS_SHELL_DISABLED){
            if(fd <= 2){ // Is Shell file 
                return 0;
            }
        }
        if(fd >= MAX_OPEN_FILES) return EBADF;
        atomicAcquire(&kot_LockFDList, 0);
        kot_file_t* File = kot_FDList[fd];
        atomicUnlock(&kot_LockFDList, 0);
        if(File == NULL) return EBADF;
        return ((File->ExternalData & File_Is_TTY) ? 0 : ENOTTY);
    }

    int sys_rmdir(const char *path){
        return (kot_rmdir((char*)path) != KSUCCESS);
    }

    int sys_unlinkat(int dirfd, const char *path, int flags){
        // TODO
        __ensure(!"Not implemented");
    }

    int sys_rename(const char *path, const char *new_path){
        return (kot_rename((char*)path, (char*)new_path) != KSUCCESS);
    }

    int sys_renameat(int olddirfd, const char *old_path, int newdirfd, const char *new_path){
        // TODO
        __ensure(!"Not implemented");
    }

    int sys_stat(fsfd_target fsfdt, int fd, const char *path, int flags, struct stat *statbuf){
        if(fd >= MAX_OPEN_FILES) return EBADF;
        atomicAcquire(&kot_LockFDList, 0);
        kot_file_t* File = kot_FDList[fd];
        atomicUnlock(&kot_LockFDList, 0);
        if(File == NULL) return EBADF;

        kot_srv_storage_callback_t* CallbackFileSize = kot_Srv_Storage_Getfilesize(File, true);
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