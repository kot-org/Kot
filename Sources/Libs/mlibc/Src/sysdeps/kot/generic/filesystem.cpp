#include <stdio.h>
#include <dirent.h>
#include <string.h>
#include <stdlib.h>
#include <kot/sys.h>
#include <sys/ioctl.h>
#include <bits/ensure.h>
#include <frg/vector.hpp>
#include <mlibc/debug.hpp>
#include <kot/descriptor.h>
#include <mlibc/allocator.hpp>
#include <kot/uisd/srvs/time.h>
#include <mlibc/all-sysdeps.hpp>
#include <abi-bits/seek-whence.h>
#include <kot/uisd/srvs/storage.h>

uint64_t FSExecFlags;
int DescriptorInitialization;

int kot_InitFS(uint64_t Flags, int DescriptorInitializationReturnValue){
    FSExecFlags = Flags;
    DescriptorInitialization = DescriptorInitializationReturnValue;
    return 0;
}

extern "C" int kot_OpenShellFile(const char *pathname){
    if(DescriptorInitialization == 0){
        // Descriptors isn't initialized yet
        kot_file_t* Shell = kot_fopenmf((char*)pathname, 0, O_CREAT | O_RDWR);

        if(Shell == NULL) return -1;

        kot_descriptor_t Descriptor{
            .Size = sizeof(kot_file_t),
            .Data = Shell,
            .Type = KOT_DESCRIPTOR_TYPE_FILE,
        };
        kot_ModifyDescriptor(0, &Descriptor); // stdin
        kot_ModifyDescriptor(1, &Descriptor); // stdout
        kot_ModifyDescriptor(2, &Descriptor); // stderr
        free(Shell);
    }

    return 0;
}

namespace mlibc{
    int sys_open(const char *pathname, int flags, mode_t mode, int *fd){
        kot_file_t* File = kot_fopenmf((char*)pathname, flags, mode);

        if(!File){
            *fd = 0;
            return ENOENT; 
        }

        kot_descriptor_t Descriptor{
            .Size = sizeof(kot_file_t),
            .Data = File,
            .Type = KOT_DESCRIPTOR_TYPE_FILE,
        };

        *fd = kot_SaveDescriptor(&Descriptor);

        if(*fd >= 0){
            free(File);
            return 0;
        }else{
            kot_fclose(File);
            return EMFILE;
        }
    }

    int sys_read(int fd, void *buf, size_t count, ssize_t *bytes_read){
        if(FSExecFlags & EXEC_FLAGS_SHELL_DISABLED){
            if(fd <= 2){ // Is Shell file 
                *bytes_read = count;
                return 0;
            }
        }
        kot_descriptor_t* Descriptor = kot_GetDescriptor(fd);
        if(Descriptor == NULL) return -EBADF;
        if(Descriptor->Type != KOT_DESCRIPTOR_TYPE_FILE) return -EBADF;
        kot_file_t* File = (kot_file_t*)Descriptor->Data;

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

        kot_descriptor_t* Descriptor = kot_GetDescriptor(fd);
        if(Descriptor == NULL) return -EBADF;
        if(Descriptor->Type != KOT_DESCRIPTOR_TYPE_FILE) return -EBADF;
        kot_file_t* File = (kot_file_t*)Descriptor->Data;

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

        kot_descriptor_t* Descriptor = kot_GetDescriptor(fd);
        if(Descriptor == NULL) return -EBADF;
        if(Descriptor->Type == KOT_DESCRIPTOR_TYPE_FILE){
            kot_file_t* File = (kot_file_t*)Descriptor->Data;

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
                    *new_offset = 0;
                    atomicUnlock(&File->Lock, 0);
                    return -EINVAL;
                }
            }
        }else if(Descriptor->Type == KOT_DESCRIPTOR_TYPE_DIRECTORY){
            kot_directory_t* Dir = (kot_directory_t*)Descriptor->Data;

            atomicAcquire(&Dir->Lock, 0);
            switch(whence){
                case SEEK_SET:{
                    Dir->Position = offset;
                    *new_offset = Dir->Position;
                    atomicUnlock(&Dir->Lock, 0);
                    return 0;
                }
                case SEEK_CUR:{
                    Dir->Position += offset;
                    *new_offset = Dir->Position;
                    atomicUnlock(&Dir->Lock, 0);
                    return 0;
                }
                case SEEK_END:{
                    atomicUnlock(&Dir->Lock, 0);
                    return 0;
                }
                default:{
                    atomicUnlock(&Dir->Lock, 0);
                    return -EINVAL;
                }
            }

        }
        return -1;
    }

    int sys_close(int fd){
        if(FSExecFlags & EXEC_FLAGS_SHELL_DISABLED){
            if(fd <= 2){ // Is Shell file 
                return 0;
            }
        }
        kot_FreeDescriptor(fd);
        return 0;
    }

    int sys_flock(int fd, int options){
        // TODO
        __ensure(!"Not implemented");
    }

    int sys_ioctl(int fd, unsigned long request, void* arg, int* result){
        kot_descriptor_t* Descriptor = kot_GetDescriptor(fd);
        if(!Descriptor){
            return EINVAL;
        }
        if(Descriptor->Type != KOT_DESCRIPTOR_TYPE_FILE){
            return EINVAL;
        }
        
        kot_file_t* File = (kot_file_t*)Descriptor->Data;

        kot_srv_storage_callback_t* Callback = kot_Srv_Storage_Ioctl(File, request, arg, true);
        int IoctlResult = static_cast<int>(Callback->Data);

        free(Callback);

        if(IoctlResult < 0){
            return -IoctlResult;
        }

        if (result){
            *result = IoctlResult;
        }

        return 0;
    }

    int sys_open_dir(const char *path, int *handle){
        kot_directory_t* Dir = kot_opendir((char*)path);

        if(!Dir){
            *handle = 0;
            return -1; 
        }

        kot_descriptor_t Descriptor{
            .Size = sizeof(kot_directory_t),
            .Data = Dir,
            .Type = KOT_DESCRIPTOR_TYPE_DIRECTORY,
        };

        *handle = kot_SaveDescriptor(&Descriptor);

        if(*handle >= 0){
            free(Dir);
            return 0;
        }else{
            kot_closedir(Dir);
            return -1;
        }

        return 0;
    }

    int sys_read_entries(int handle, void *buffer, size_t max_size, size_t *bytes_read){
        kot_descriptor_t* Descriptor = kot_GetDescriptor(handle);
        if(Descriptor == NULL) return -EBADF;
        if(Descriptor->Type != KOT_DESCRIPTOR_TYPE_DIRECTORY) return -EBADF;
        kot_directory_t* Dir = (kot_directory_t*)Descriptor->Data;

        kot_directory_entry_t* KotDirEntry = kot_readdir(Dir);

        if(KotDirEntry != NULL){
            dirent* DirEntry = (dirent*)buffer;

            DirEntry->d_ino = KotDirEntry->NextEntryPosition;
            DirEntry->d_off = 0;
            DirEntry->d_reclen = sizeof(dirent);
            DirEntry->d_type = KotDirEntry->IsFile ? DT_REG : DT_DIR;
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

        int Id;
        int Result;

        if(!sys_ioctl(fd, TIOCGPTN, &Id, &Result)){
            return 0;
        }

        return ENOTTY;
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
        if(fsfdt == fsfd_target::none){
            return -1;
        }

        bool IsOpenWithPath = false;
        if(fsfdt == fsfd_target::path || fsfdt == fsfd_target::fd_path){
            if(path){
                if(path[0]){
                    IsOpenWithPath = true;
                    int e = sys_open(path, flags, 0666, &fd);
                    if(e){
                        e = sys_open_dir(path, &fd);
                        if(e){
                            return -1;
                        }
                    }
                }
            }
        }

        kot_descriptor_t* Descriptor = kot_GetDescriptor(fd);
        if(Descriptor == NULL) return -EBADF;
        if(Descriptor->Type == KOT_DESCRIPTOR_TYPE_FILE){
            kot_file_t* File = (kot_file_t*)Descriptor->Data;

            kot_srv_storage_callback_t* CallbackFileSize = kot_Srv_Storage_Getfilesize(File, true);
            if(CallbackFileSize->Status != KSUCCESS){
                free(CallbackFileSize);
                if(IsOpenWithPath){
                    sys_close(fd);
                }
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

            if(IsOpenWithPath){
                sys_close(fd);
            }
            return 0;
        }else if(Descriptor->Type == KOT_DESCRIPTOR_TYPE_DIRECTORY){
            kot_directory_t* Dir = (kot_directory_t*)Descriptor->Data;
            // TODO
            statbuf->st_size = 0;
            statbuf->st_dev = 0;
            statbuf->st_ino = 0;
            statbuf->st_mode = 0;
            statbuf->st_nlink = 0;
            statbuf->st_uid = 0;
            statbuf->st_gid = 0;
            statbuf->st_rdev = 0;
            statbuf->st_blksize = 0;
            statbuf->st_blocks = 0;

            if(IsOpenWithPath){
                sys_close(fd);
            }
            return 0;
        }else{
            return -1;
        }
    }

    int sys_fcntl(int fd, int request, va_list args, int *result_value){
        // TODO
        *result_value = 0;
        infoLogger() << "mlibc: sys_fcntl unsupported request (" << request << ")" << frg::endlog;
        return -1;
    }

    int sys_getcwd(char *buffer, size_t size){
        kot_srv_storage_callback_t* Callback = kot_Srv_Storage_GetCWD(true);
        if((Callback->Size + 1) > size){
            free(Callback);
            return -ERANGE;
        }
        memcpy(buffer, (void*)Callback->Data, Callback->Size);
        buffer[Callback->Size] = '\0';
        free(Callback);
        return 0;
    }

    int sys_chdir(const char *path){
        char AbsolutePathBuffer[PATH_MAX];
        char* AbsolutePath = realpath(path, AbsolutePathBuffer);
        if(!AbsolutePath){
            return -1;
        }
        kot_srv_storage_callback_t* Callback = kot_Srv_Storage_SetCWD((char*)AbsolutePath, true);
        KResult Status = Callback->Status;
        free(Callback);
        return (Status != KSUCCESS);
    }

    int sys_tcgetattr(int fd, struct termios *attr){
        int result;

        if (int e = sys_ioctl(fd, TCGETS, (void *)attr, &result); e)
            return e;

        return 0;
    }

    int sys_tcsetattr(int fd, int optional_action, const struct termios *attr){
        if (optional_action)
            mlibc::infoLogger()
                << "mlibc: warning: sys_tcsetattr ignores optional_action"
                << frg::endlog;

        int result;

        if (int e = sys_ioctl(fd, TCSETSF, (void *)attr, &result); e)
            return e;

        return 0;
    }
}