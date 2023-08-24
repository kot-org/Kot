#include <core.h>

#define VFS_BRIDGE(function_name)

static char* empty_path = "";
static hashmap_t* vfs_hashmap = NULL;
static spinlock_t vfs_lock = {};

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

void init_vfs(void){
    vfs_hashmap = hashmap_create(32);
}

int mount_vfs(const char* fs_mount_name, fs_t* new_fs){
    spinlock_acquire(&vfs_lock);

    fs_t* fs = malloc(sizeof(fs_t));
    memcpy(fs, new_fs, sizeof(fs_t));

    int err = add_fs(fs_mount_name, fs);

    spinlock_release(&vfs_lock);
    return err;
}

int unmount_vfs(const char* fs_mount_name){
    spinlock_acquire(&vfs_lock);

    fs_t* fs = get_fs(fs_mount_name);
    free(fs);

    int err = remove_fs(fs_mount_name);

    spinlock_release(&vfs_lock);

    return err;
}

int get_fs_mount_name_from_path(const char* path, char** fs_mount_name, char** fs_relative_path){
    char* fs_relative_path_tmp = strchr(path, '/');
    *fs_mount_name = (char*)path;
    if(fs_relative_path_tmp != NULL){
        *fs_relative_path_tmp = '\0';
        fs_relative_path_tmp++;
        *fs_relative_path = fs_relative_path_tmp;
    }else{
        *fs_relative_path = empty_path;
    }
    return 0;
}

int get_fs_and_relative_path(const char* path, char** fs_relative_path, fs_t** fs){
    char* fs_mount_name;
    int err = get_fs_mount_name_from_path(path, &fs_mount_name, fs_relative_path);
    if(err){
        return err;
    }

    if((*fs = get_fs(fs_mount_name)) == NULL){
        return EINVAL;
    }

    return 0;
}

/* vfs functions */

/* file */
int file_remove(const char* path){
    char* fs_relative_path;
    fs_t* fs;

    int error = get_fs_and_relative_path(path, &fs_relative_path, &fs);

    if(error){
        return error;
    }

    return fs->file_remove(fs_relative_path);    
}

kernel_file_t* vfs_open(const char* path, int flags, mode_t mode, int* error){
    char* fs_relative_path;
    fs_t* fs;

    *error = get_fs_and_relative_path(path, &fs_relative_path, &fs);

    if(*error){
        return NULL;
    }

    return fs->file_open(fs_relative_path, flags, mode, error);
}

/* directory */
int dir_create(const char* path, mode_t mode){
    char* fs_relative_path;
    fs_t* fs;

    int error = get_fs_and_relative_path(path, &fs_relative_path, &fs);

    if(error){
        return error;
    }

    return fs->dir_create(fs_relative_path, mode);
}

int dir_create_at(kernel_dir_t* parent_dir, const char* path, mode_t mode){
    return parent_dir->fs->dir_create_at(parent_dir, path, mode);
}

int dir_remove(const char* path){
    char* fs_relative_path;
    fs_t* fs;

    int error = get_fs_and_relative_path(path, &fs_relative_path, &fs);

    if(error){
        return error;
    }

    return fs->dir_remove(fs_relative_path);
}

kernel_dir_t* dir_open(const char* path, int* error){
    char* fs_relative_path;
    fs_t* fs;

    *error = get_fs_and_relative_path(path, &fs_relative_path, &fs);

    if(*error){
        return NULL;
    }

    return fs->dir_open(path, error);
}

/* file and directory */
int rename(const char* old_path, const char* new_path){
    char* old_fs_relative_path;
    fs_t* old_fs;

    int error = get_fs_and_relative_path(old_path, &old_fs_relative_path, &old_fs);

    if(error){
        return error;
    }
    char* new_fs_relative_path;
    fs_t* new_fs;

    error = get_fs_and_relative_path(new_path, &new_fs_relative_path, &new_fs);

    if(error){
        return error;
    }

    if(old_fs != new_fs){
        // TODO
        return ENOSYS;
    }

    return old_fs->rename(old_fs_relative_path, new_fs_relative_path);
}

/* file and directory */
int link(const char* src_path, const char* dst_path){
    char* src_fs_relative_path;
    fs_t* src_fs;

    int error = get_fs_and_relative_path(src_path, &src_fs_relative_path, &src_fs);

    if(error){
        return error;
    }
    char* dst_fs_relative_path;
    fs_t* dst_fs;

    error = get_fs_and_relative_path(dst_path, &dst_fs_relative_path, &dst_fs);

    if(error){
        return error;
    }

    if(src_fs != dst_fs){
        // TODO
        return ENOSYS;
    }

    return src_fs->rename(src_fs_relative_path, dst_fs_relative_path);
}

int unlink_at(kernel_dir_t* parent_dir, const char* path, mode_t mode){
    return parent_dir->fs->unlink_at(parent_dir, path, mode);
}