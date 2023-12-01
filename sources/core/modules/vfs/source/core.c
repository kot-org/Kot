#include <core.h>

#define VFS_BRIDGE(function_name)

static hashmap_t* vfs_hashmap = NULL;
static spinlock_t vfs_lock = SPINLOCK_INIT;
static uint64_t friendly_name_count = 0;
static uint64_t removable_friendly_name_count = 0;
static spinlock_t friendly_name_lock = SPINLOCK_INIT;

static fs_t* get_fs(const char* fs_mount_name){
	return (fs_t*)hashmap_get(vfs_hashmap, fs_mount_name);
}

static int add_fs(const char* fs_mount_name, fs_t* fs){
	hashmap_set(vfs_hashmap, fs_mount_name, fs);
    return 0;
}

static int remove_fs(const char* fs_mount_name){
	hashmap_remove(vfs_hashmap, fs_mount_name);
    return 0;
}

static int get_fs_mount_name_from_path(vfs_ctx_t* ctx, const char* path, char* fs_mount_name, char* fs_relative_path){
    char full_path[VFS_MAX_PATH_SIZE + 1];
    size_t path_size = strlen(path);
    if(path[0] != '/'){
        full_path[0] = '\0';
        if(ctx != NULL){
            spinlock_acquire(&ctx->cwd_lock);
            if(ctx->cwd_size + path_size > VFS_MAX_PATH_SIZE){
                spinlock_release(&ctx->cwd_lock);
                return ENAMETOOLONG;
            }
            strcat(full_path, ctx->cwd);
            spinlock_release(&ctx->cwd_lock);
            strcat(full_path, path);
            path = full_path;
        }else{
            if(path_size > VFS_MAX_PATH_SIZE){
                return ENAMETOOLONG;
            }
        }
    }else{
        path += sizeof((char)'/');
        if(path_size > VFS_MAX_PATH_SIZE){
            return ENAMETOOLONG;
        }
    }
    char* fs_relative_path_tmp = strchr(path, '/');
    if(fs_relative_path_tmp != NULL){
        fs_mount_name[0] = '/';
        size_t fs_mount_name_size = (size_t)((uintptr_t)fs_relative_path_tmp - (uintptr_t)path);
        strncpy(fs_mount_name + sizeof((char)'/'), path, fs_mount_name_size);
        strcpy(fs_relative_path, fs_relative_path_tmp);
    }else{
        fs_relative_path[0] = '\0';
        fs_mount_name[0] = '/';
        strncpy(fs_mount_name + sizeof((char)'/'), path, path_size);
    }
    return 0;
}

static int get_fs_and_relative_path(vfs_ctx_t* ctx, const char* path, char* fs_relative_path, fs_t** fs){
    char fs_mount_name[VFS_MAX_PATH_SIZE + 1];
    int err = get_fs_mount_name_from_path(ctx, path, fs_mount_name, fs_relative_path);
    if(err){
        return err;
    }

    spinlock_acquire(&vfs_lock);

    if((*fs = get_fs(fs_mount_name)) == NULL){
        spinlock_release(&vfs_lock);
        return EINVAL;
    }

    spinlock_release(&vfs_lock);

    return 0;
}

static char* get_chars_with_number(char* prefix, uint64_t num){
    size_t suffix_size = num / 26 + 1;
    size_t prefix_size = strlen(prefix);

    char* str = malloc(prefix_size + suffix_size + 1);
    str[prefix_size + suffix_size] = '\0';
    memcpy(str, prefix, prefix_size);

    for(size_t i = 0; i < suffix_size; i++){
        str[prefix_size + i] = (num % 26) + 'a';
        num /= 26;
    }
    return str;
}

/* vfs special fs implementation */

int vfs_return_not_implemented(void){
    return ENOSYS;
}

kernel_file_t* vfs_file_open_not_implemented(fs_t* ctx, const char* path, int flags, mode_t mode, int* error){
    *error = ENOSYS;
    return NULL;
}

int vfs_interface_dir_get_directory_entries(void* buffer, size_t max_size, size_t* bytes_read, kernel_dir_t* dir){
    uint64_t max_entry_count = (uint64_t)(max_size / sizeof(dirent_t));
    dirent_t* entry = (dirent_t*)buffer;
    uint64_t entry_index = dir->seek_position;
    uint64_t current_entry_count = 0;

    spinlock_acquire(&vfs_lock);
    uint64_t entry_index_end = hashmap_get_end(vfs_hashmap);

    while(entry_index < entry_index_end && current_entry_count < max_entry_count){
        char* name = hashmap_get_key(vfs_hashmap, entry_index);
        if(name != NULL){
            name += sizeof((char)'/');
            if(name[0] != '\0'){
                entry->d_ino = (ino_t)entry_index;
                entry->d_off = (off_t)entry_index;
                entry->d_reclen = sizeof(dirent_t);
                entry->d_type = DT_DIR;
                size_t size_name_to_copy = MIN(strlen(name), sizeof(entry->d_name) - 1);
                strncpy(entry->d_name, name, size_name_to_copy);
                entry->d_name[size_name_to_copy] = '\0';

                current_entry_count++;

                entry = (dirent_t*)((off_t)entry + entry->d_reclen);
            }
        }
        entry_index++;
    }

    spinlock_release(&vfs_lock);

    dir->seek_position = entry_index;

    *bytes_read = current_entry_count * sizeof(dirent_t);

    return 0;
}

int vfs_interface_dir_create_at(struct kernel_dir_t* dir, const char* path, mode_t mode){
    return ENOSYS;
}

int vfs_interface_dir_unlink_at(struct kernel_dir_t* dir, const char* path, int flags){
    return ENOSYS;
}

int vfs_interface_dir_stat(int flags, struct stat* statbuf, struct kernel_dir_t* dir){
    memset(statbuf, 0, sizeof(struct stat));
    statbuf->st_mode = S_IFDIR;
    return 0;
}

int vfs_interface_dir_close(struct kernel_dir_t* dir){
    free(dir);
    return 0;
}

kernel_dir_t* vfs_interface_dir_open(fs_t* ctx, const char* path, int* error){
    kernel_dir_t* dir = malloc(sizeof(kernel_dir_t));

    dir->fs_ctx = ctx;
    dir->seek_position = hashmap_get_start(vfs_hashmap);
    dir->get_directory_entries = &vfs_interface_dir_get_directory_entries;
    dir->create_at = &vfs_interface_dir_create_at;
    dir->unlink_at = &vfs_interface_dir_unlink_at;
    dir->stat = &vfs_interface_dir_stat;
    dir->close = &vfs_interface_dir_close;

    return dir;
}

int vfs_interface_stat(struct fs_t* ctx, const char* path, int flags, struct stat* statbuf){
    memset(statbuf, 0, sizeof(struct stat));
    statbuf->st_mode = S_IFDIR;
    return 0;
}

void vfs_init(void){
    vfs_hashmap = hashmap_create(32);
    fs_t* fs_vfs = malloc(sizeof(fs_t));
    fs_vfs->file_remove = (file_remove_fs_t)&vfs_return_not_implemented;
    fs_vfs->file_open = &vfs_file_open_not_implemented;
    fs_vfs->dir_create = (dir_create_fs_t)&vfs_return_not_implemented;
    fs_vfs->dir_remove = (dir_remove_fs_t)&vfs_return_not_implemented;
    fs_vfs->dir_open = &vfs_interface_dir_open;
    fs_vfs->rename = (rename_fs_t)&vfs_return_not_implemented;
    fs_vfs->link = (link_fs_t)&vfs_return_not_implemented;
    fs_vfs->stat = &vfs_interface_stat;
    local_mount_fs("/", fs_vfs);
    local_mount_fs("/.", fs_vfs);
}

char* request_friendly_fs_mount_name(bool is_removable){
    char* str;
    spinlock_acquire(&friendly_name_lock);
    if(is_removable){
        str = get_chars_with_number("/rd", removable_friendly_name_count); // rd : removable device
        removable_friendly_name_count++;
    }else{
        str = get_chars_with_number("/sd", friendly_name_count); // sd : static device
        friendly_name_count++;
    }
    spinlock_release(&friendly_name_lock);
    return str;
}

int free_friendly_fs_mount_name(const char* fs_mount_name){
    free((void*)fs_mount_name);
    return 0;
}

/* Warning : this function should be called by a parent which possess the vfs_lock */
int local_mount_fs(const char* fs_mount_name, fs_t* new_fs){
    if(get_fs(fs_mount_name) != NULL){
        return EINVAL;
    }

    fs_t* fs = malloc(sizeof(fs_t));
    memcpy(fs, new_fs, sizeof(fs_t));

    size_t len_fs_mount_name = strlen(fs_mount_name);
    char* fs_mount_name_buffer = malloc(len_fs_mount_name + 1);
    strncpy(fs_mount_name_buffer, fs_mount_name, len_fs_mount_name);
    int err = add_fs(fs_mount_name_buffer, fs);

    return err;
}

int mount_fs(const char* fs_mount_name, fs_t* new_fs){
    spinlock_acquire(&vfs_lock);
    
    if(get_fs(fs_mount_name) != NULL){
        spinlock_release(&vfs_lock);
        return EINVAL;
    }

    fs_t* fs = malloc(sizeof(fs_t));
    memcpy(fs, new_fs, sizeof(fs_t));

    size_t len_fs_mount_name = strlen(fs_mount_name);
    char* fs_mount_name_buffer = malloc(len_fs_mount_name + 1);
    strncpy(fs_mount_name_buffer, fs_mount_name, len_fs_mount_name);
    int err = add_fs(fs_mount_name_buffer, fs);
    
    system_tasks_mount(new_fs);

    spinlock_release(&vfs_lock);
    return err;
}

int unmount_fs(const char* fs_mount_name){
    spinlock_acquire(&vfs_lock);

    fs_t* fs = get_fs(fs_mount_name);
    if(fs == NULL){
        spinlock_release(&vfs_lock);
        return EINVAL;
    }

    free(fs);
    
    char* fs_mount_name_buffer = hashmap_get_key_with_key(vfs_hashmap, fs_mount_name);

    int err = remove_fs(fs_mount_name);

    spinlock_release(&vfs_lock);

    return err;
}

/* vfs functions */

/* file */
int file_remove(vfs_ctx_t* ctx, const char* path){
    char fs_relative_path[VFS_MAX_PATH_SIZE + 1];
    fs_t* fs;

    int error = get_fs_and_relative_path(ctx, path, fs_relative_path, &fs);

    if(error){
        return error;
    }

    return fs->file_remove(fs, fs_relative_path);    
}

kernel_file_t* file_open(vfs_ctx_t* ctx, const char* path, int flags, mode_t mode, int* error){
    char fs_relative_path[VFS_MAX_PATH_SIZE + 1];
    fs_t* fs;

    *error = get_fs_and_relative_path(ctx, path, fs_relative_path, &fs);

    if(*error){
        return NULL;
    }

    return fs->file_open(fs, fs_relative_path, flags, mode, error);
}

/* directory */
int dir_create(vfs_ctx_t* ctx, const char* path, mode_t mode){
    char fs_relative_path[VFS_MAX_PATH_SIZE + 1];
    fs_t* fs;

    int error = get_fs_and_relative_path(ctx, path, fs_relative_path, &fs);

    if(error){
        return error;
    }

    return fs->dir_create(fs, fs_relative_path, mode);
}

int dir_remove(vfs_ctx_t* ctx, const char* path){
    char fs_relative_path[VFS_MAX_PATH_SIZE + 1];
    fs_t* fs;

    int error = get_fs_and_relative_path(ctx, path, fs_relative_path, &fs);

    if(error){
        return error;
    }

    return fs->dir_remove(fs, fs_relative_path);
}

kernel_dir_t* dir_open(vfs_ctx_t* ctx, const char* path, int* error){
    char fs_relative_path[VFS_MAX_PATH_SIZE + 1];
    fs_t* fs;

    *error = get_fs_and_relative_path(ctx, path, fs_relative_path, &fs);

    if(*error){
        return NULL;
    }

    return fs->dir_open(fs, fs_relative_path, error);
}

/* file and directory */
int vfs_rename(vfs_ctx_t* ctx, const char* old_path, const char* new_path){
    char old_fs_relative_path[VFS_MAX_PATH_SIZE + 1];
    fs_t* old_fs;

    int error = get_fs_and_relative_path(ctx, old_path, old_fs_relative_path, &old_fs);

    if(error){
        return error;
    }
    char new_fs_relative_path[VFS_MAX_PATH_SIZE + 1];
    fs_t* new_fs;

    error = get_fs_and_relative_path(ctx, new_path, new_fs_relative_path, &new_fs);

    if(error){
        return error;
    }

    if(old_fs != new_fs){
        // TODO
        return EXDEV;
    }

    return old_fs->rename(old_fs, old_fs_relative_path, new_fs_relative_path);
}

/* file and directory */
int vfs_link(vfs_ctx_t* ctx, const char* src_path, const char* dst_path){
    char src_fs_relative_path[VFS_MAX_PATH_SIZE + 1];
    fs_t* src_fs;

    int error = get_fs_and_relative_path(ctx, src_path, src_fs_relative_path, &src_fs);

    if(error){
        return error;
    }
    char dst_fs_relative_path[VFS_MAX_PATH_SIZE + 1];
    fs_t* dst_fs;

    error = get_fs_and_relative_path(ctx, dst_path, dst_fs_relative_path, &dst_fs);

    if(error){
        return error;
    }

    if(src_fs != dst_fs){
        // TODO
        return EXDEV;
    }

    return src_fs->link(src_fs, src_fs_relative_path, dst_fs_relative_path);
}

int vfs_stat(vfs_ctx_t* ctx, const char* path, int flags, struct stat* statbuf){
    char fs_relative_path[VFS_MAX_PATH_SIZE + 1];
    fs_t* fs;

    int error = get_fs_and_relative_path(ctx, path, fs_relative_path, &fs);
    
    if(error){
        return error;
    }

    return fs->stat(fs, fs_relative_path, flags, statbuf);    
}