#include <core.h>

#define DISK_CFG_PATH "/system/disk/disk.cfg"
#define INITRD_DISK_CFG_PATH "/initrd"DISK_CFG_PATH

static bool is_system_fs_found = false;
static void* initrd_disk_cfg_buffer = NULL;
static size_t initrd_disk_cfg_size = 0;
static fs_t* system_fs;

/* vfs functions */

/* file */
int system_file_remove(fs_t* ctx, const char* path){
    char fs_relative_path[VFS_MAX_PATH_SIZE];
    strcpy(fs_relative_path, ctx->internal_data);
    strcat(fs_relative_path, path);

    return system_fs->file_remove(system_fs, fs_relative_path);    
}

kernel_file_t* system_file_open(fs_t* ctx, const char* path, int flags, mode_t mode, int* error){
    char fs_relative_path[VFS_MAX_PATH_SIZE];
    strcpy(fs_relative_path, ctx->internal_data);
    strcat(fs_relative_path, path);

    return system_fs->file_open(system_fs, fs_relative_path, flags, mode, error);
}

/* directory */
int system_dir_create(fs_t* ctx, const char* path, mode_t mode){
    char fs_relative_path[VFS_MAX_PATH_SIZE];
    strcpy(fs_relative_path, ctx->internal_data);
    strcat(fs_relative_path, path);

    return system_fs->dir_create(system_fs, fs_relative_path, mode);
}

int system_dir_remove(fs_t* ctx, const char* path){
    char fs_relative_path[VFS_MAX_PATH_SIZE];
    strcpy(fs_relative_path, ctx->internal_data);
    strcat(fs_relative_path, path);

    return system_fs->dir_remove(system_fs, fs_relative_path);
}

kernel_dir_t* system_dir_open(fs_t* ctx, const char* path, int* error){
    char fs_relative_path[VFS_MAX_PATH_SIZE];
    strcpy(fs_relative_path, ctx->internal_data);
    strcat(fs_relative_path, path);

    return system_fs->dir_open(system_fs, fs_relative_path, error);
}

/* file and directory */
int system_rename(fs_t* ctx, const char* old_path, const char* new_path){
    char old_fs_relative_path[VFS_MAX_PATH_SIZE];
    strcpy(old_fs_relative_path, ctx->internal_data);
    strcat(old_fs_relative_path, old_path);

    char new_fs_relative_path[VFS_MAX_PATH_SIZE];
    strcpy(new_fs_relative_path, ctx->internal_data);
    strcat(new_fs_relative_path, new_path);

    return system_fs->rename(system_fs, old_fs_relative_path, new_fs_relative_path);
}

/* file and directory */
int system_link(fs_t* ctx, const char* src_path, const char* dst_path){
    char src_fs_relative_path[VFS_MAX_PATH_SIZE];
    strcpy(src_fs_relative_path, ctx->internal_data);
    strcat(src_fs_relative_path, src_path);

    char dst_fs_relative_path[VFS_MAX_PATH_SIZE];
    strcpy(dst_fs_relative_path, ctx->internal_data);
    strcat(dst_fs_relative_path, dst_path);

    return system_fs->link(system_fs, src_fs_relative_path, dst_fs_relative_path);
}

static bool check_system_file(fs_t* fs){
    int error = 0;

    kernel_file_t* disk_file = fs->file_open(fs, DISK_CFG_PATH, 0, 0, &error);

    if(!error){
        if(initrd_disk_cfg_size == disk_file->file_size_initial){
            void* disk_cfg_buffer = malloc(initrd_disk_cfg_size);
            size_t size_read;

            disk_file->read(disk_cfg_buffer, initrd_disk_cfg_size, &size_read, disk_file);

            assert(initrd_disk_cfg_size == size_read);

            char* line = (char*)disk_cfg_buffer;
            
            while(line != NULL){
                char* data = strchr(line, '=') + sizeof(char);
                char* end_of_line = strchr(line, '\n');
                char* current_line = line;
                if(end_of_line){
                    *end_of_line = '\0';
                    line = end_of_line + sizeof(char);
                }else{
                    line = NULL;
                }

                if(strstr(current_line, "DISK_SYSTEM_UUID=")){
                    uintptr_t offset = (uintptr_t)data - (uintptr_t)disk_cfg_buffer;
                    if(!strncmp(data, (char*)((uintptr_t)initrd_disk_cfg_buffer + offset), strlen(data))){
                        disk_file->close(disk_file);
                        free(disk_cfg_buffer);
                        return true;
                    }
                }
            }

            disk_file->close(disk_file);
            free(disk_cfg_buffer);
        }
    }
    return false;
}

static bool is_system_fs(fs_t* fs){
    bool is_system_disk = false;
    if(!is_system_fs_found){
        is_system_disk = check_system_file(fs);
        if(is_system_disk){
            is_system_fs_found = is_system_disk;
        }
    }
    return is_system_disk;
}

void system_tasks_mount(fs_t* fs){
    if(is_system_fs(fs)){
        system_fs = fs;

        char* line = (char*)initrd_disk_cfg_buffer;
        
        while(line != NULL){
            char* data = strchr(line, '=') + sizeof(char);
            char* end_of_line = strchr(line, '\n');
            char* current_line = line;
            if(end_of_line){
                *end_of_line = '\0';
                line = end_of_line + sizeof(char);
            }else{
                line = NULL;
            }

            if(strstr(current_line, "ROOT_DIR_PATH=")){
                fs_t* fs_vfs = malloc(sizeof(fs_t));
                /* note we don't need to copy string from data to internal data because data won't be free */
                fs_vfs->internal_data = data;
                fs_vfs->file_remove = &system_file_remove;
                fs_vfs->file_open = &system_file_open;
                fs_vfs->dir_create = &system_dir_create;
                fs_vfs->dir_remove = &system_dir_remove;
                fs_vfs->dir_open = &system_dir_open;
                fs_vfs->rename = &system_rename;
                fs_vfs->link = &system_link;
                local_mount_fs(data, fs_vfs);         
            }
        }
    }
}

void system_tasks_init(void){
    int error = 0;

    /* we don't use f_open function because the vfs is not fully initialized yet */
    kernel_file_t* disk_file = file_open(KERNEL_VFS_CTX, INITRD_DISK_CFG_PATH, 0, 0, &error);

    if(!error){
        initrd_disk_cfg_size = disk_file->file_size_initial;
        initrd_disk_cfg_buffer = malloc(initrd_disk_cfg_size);

        size_t size_read;

        disk_file->read(initrd_disk_cfg_buffer, initrd_disk_cfg_size, &size_read, disk_file);

        assert(initrd_disk_cfg_size == size_read);
    }else{
        panic("Error while opening %s, error code : %d", INITRD_DISK_CFG_PATH, error);
    }
}