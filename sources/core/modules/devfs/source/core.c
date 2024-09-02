#include <core.h>

static devfs_directory_internal_t* devfs_creat_new_directory_field(void){
    devfs_directory_internal_t* directory_internal = malloc(sizeof(devfs_directory_internal_t));
    directory_internal->entries = vector_create();
    return directory_internal;
}

static int devfs_set_entry_name(devfs_directory_entry_t* entry, char* new_name){
    size_t new_name_len = strlen(new_name);
    char* new_name_buffer = malloc(new_name_len + 1);
    strncpy(new_name_buffer, new_name, new_name_len);
    entry->name = new_name_buffer;
    return 0;
}

static char* devfs_get_entry_name(devfs_directory_entry_t* entry){
    return entry->name;
}

static int devfs_change_entry_name(devfs_directory_entry_t* entry, char* new_name){
    char* old_name = devfs_get_entry_name(entry);
    assert(!devfs_set_entry_name(entry, new_name));
    free(old_name);
    return 0;
}

static int devfs_add_entry(devfs_directory_entry_t* parent, devfs_directory_entry_t* entry){
    if(parent->is_file){
        return EACCES;
    }

    entry->parent = parent;

    entry->index = vector_push(parent->data.directory->entries, entry);

    return 0;
}

static int devfs_remove_entry(devfs_directory_entry_t* entry){
    if(entry->parent == NULL){
        return EACCES;
    }

    assert(!entry->parent->is_file);

    vector_remove(entry->parent->data.directory->entries, entry->index);

    return 0;
}

static devfs_directory_entry_t* devfs_get_entry(devfs_directory_entry_t* entry, const char* name){
    assert(!entry->is_file);

    if(!strcmp(name, ".")){
        return entry;
    }

    if(!strcmp(name, "..")){
        return entry->parent;
    }

    for(uint64_t i = 0; i < entry->data.directory->entries->length; i++){
        devfs_directory_entry_t* entry_read = vector_get(entry->data.directory->entries, i);
        if(!strcmp(entry_read->name, name)){
            return entry_read;
        }
    }

    return NULL;
}

static int devfs_create_directory_entry_to_parent(devfs_directory_entry_t* parent, char* entry_name){
    devfs_directory_entry_t* entry = malloc(sizeof(devfs_directory_entry_t));

    entry->data.directory = devfs_creat_new_directory_field();

    entry->parent = parent;

    entry->is_file = false;

    assert(!devfs_set_entry_name(entry, entry_name));

    assert(!devfs_add_entry(parent, entry));

    return 0;
}

static devfs_directory_entry_t* devfs_get_entry_with_path(devfs_directory_entry_t* entry, const char* path, int* error){
    if(entry->is_file){
        *error = ENOENT;
        return NULL;
    }
    
    char* entry_name = (char*)path;
    char* next_entry_name = strchr(entry_name, '/');

    while(next_entry_name != NULL){
        *next_entry_name = '\0';

        entry = devfs_get_entry(entry, entry_name);

        *next_entry_name = '/';

        if(entry == NULL){
            *error = ENOENT;
            return NULL;
        }

        if(entry->is_file){
            *error = ENOENT;
            return NULL;
        }

        entry_name = next_entry_name + 1;
        next_entry_name = strchr(entry_name, '/');
    }

    if(entry_name[0] != '\0'){
        entry = devfs_get_entry(entry, entry_name);
    }

    if(entry == NULL){
        *error = ENOENT;
        return NULL;        
    }

    *error = 0;
    return entry;
}


static devfs_directory_entry_t* devfs_get_entry_with_path_from_root(devfs_context_t* ctx, const char* path, int* error){
    return devfs_get_entry_with_path(ctx->root_entry, path, error);
}

static devfs_directory_entry_t* devfs_find_last_directory_with_path(devfs_directory_entry_t* dir, const char* path, char** entry_name_out, int* error){
    if(dir->is_file){
        *error = ENOENT;
        return NULL;
    }
    
    char* entry_name = (char*)path;
    char* next_entry_name = strchr(entry_name, '/');

    while(next_entry_name != NULL){
        *next_entry_name = '\0';

        dir = devfs_get_entry(dir, entry_name);

        *next_entry_name = '/';

        if(dir == NULL){
            *error = ENOENT;
            return NULL;
        }

        if(dir->is_file){
            *error = ENOENT;
            return NULL;
        }
        
        entry_name = next_entry_name + 1;
        next_entry_name = strchr(entry_name, '/');
    }

    if(entry_name_out != NULL){
        *entry_name_out = entry_name;
    }

    return dir;
}

static devfs_directory_entry_t* devfs_find_last_directory_with_path_from_root(devfs_context_t* ctx, const char* path, char** entry_name_out, int* error){
    return devfs_find_last_directory_with_path(ctx->root_entry, path, entry_name_out, error);
}

static devfs_file_internal_t* devfs_open_file(devfs_context_t* ctx, const char* path, int flags, mode_t mode, int* error){
    devfs_directory_entry_t* entry = devfs_get_entry_with_path_from_root(ctx, path, error);

    if(*error){
        return NULL;
    }

    if(!entry->is_file){
        *error = ENOENT;
        return NULL;
    }

    return entry->data.file;
}

static devfs_directory_internal_t* devfs_open_dir(devfs_context_t* ctx, const char* path, int* error){
    devfs_directory_entry_t* entry = devfs_get_entry_with_path_from_root(ctx, path, error);

    if(*error){
        return NULL;
    }

    if(entry->is_file){
        *error = ENOENT;
        return NULL;
    }

    return entry->data.directory;
}


/* interface */
static inline char* devfs_interface_convert_path(char* str){
    size_t str_len = strlen(str);
    if(str[str_len - 1] == '/'){
        str[str_len - 1] = '\0';
    }
    if(str[0] == '/'){
        return str + sizeof((char)'/');
    }else{
        return str;
    }
}

int devfs_interface_file_remove(fs_t* ctx, const char* path){
    int error = 0;

    devfs_directory_entry_t* entry = devfs_get_entry_with_path_from_root(ctx->internal_data, path, &error);

    if(error){
        return error;
    }

    if(entry->is_file){
        return ENOENT;
    }

    devfs_remove_entry(entry);

    free(entry);

    return 0;
}

struct kernel_file_t* devfs_interface_file_open(struct fs_t* ctx, const char* path, int flags, mode_t mode, int* error){
    devfs_file_internal_t* devfs_file = devfs_open_file((devfs_context_t*)ctx->internal_data, devfs_interface_convert_path((char*)path), flags, mode, error);
    if(devfs_file == NULL){
        return NULL;
    }

    return devfs_file->open_handler(ctx, path, flags, mode, error);
}

int devfs_interface_dir_create(struct fs_t* ctx, const char* path, mode_t mode){
    devfs_context_t* devfs_ctx = (devfs_context_t*)ctx->internal_data;

    int error = 0;
    char* entry_name;
    devfs_directory_entry_t* parent = devfs_find_last_directory_with_path_from_root(devfs_ctx, path, &entry_name, &error);

    if(error){
        return error;
    }

    devfs_create_directory_entry_to_parent(parent, entry_name);

    return 0;
}

int devfs_interface_dir_remove(struct fs_t* ctx, const char* path){
    int error = 0;

    devfs_directory_entry_t* entry = devfs_get_entry_with_path_from_root(ctx->internal_data, path, &error);

    if(error){
        return error;
    }

    if(!entry->is_file){
        return ENOENT;
    }

    if(entry->data.directory->entries > 0){
        return EACCES;
    }

    error = devfs_remove_entry(entry);

    if(error){
        return error;
    }

    free(entry);
    
    return 0;
}

int devfs_interface_dir_get_directory_entries(void* buffer, size_t max_size, size_t* bytes_read, kernel_dir_t* dir){
    uint64_t max_entry_count = (uint64_t)(max_size / sizeof(dirent_t));
    dirent_t* entry = (dirent_t*)buffer;
    uint64_t entry_index = dir->seek_position;
    uint64_t current_entry_count = 0;

    devfs_directory_internal_t* devfs_dir = dir->internal_data;
    uint64_t entry_index_end = devfs_dir->entries->length;

    while(entry_index < entry_index_end && current_entry_count < max_entry_count){
        devfs_directory_entry_t* dir_entry = vector_get(devfs_dir->entries, entry_index);
        entry->d_ino = (ino_t)entry_index;
        entry->d_off = (off_t)entry_index;
        entry->d_reclen = sizeof(dirent_t);
        entry->d_type =  dir_entry->is_file ? DT_REG : DT_DIR;
        size_t size_name_to_copy = MIN(strlen(dir_entry->name), sizeof(entry->d_name) - 1);
        strncpy(entry->d_name, dir_entry->name, size_name_to_copy);
        entry->d_name[size_name_to_copy] = '\0';

        current_entry_count++;

        entry = (dirent_t*)((off_t)entry + entry->d_reclen);
        entry_index++;
    }

    dir->seek_position = entry_index;

    *bytes_read = current_entry_count * sizeof(dirent_t);

    return 0;
}

int devfs_interface_dir_create_at(struct kernel_dir_t* dir, const char* path, mode_t mode){
    return ENOSYS;
}

int devfs_interface_dir_unlink_at(struct kernel_dir_t* dir, const char* path, int flags){
    return ENOSYS;
}

int devfs_interface_dir_stat(int flags, struct stat* statbuf, struct kernel_dir_t* dir){
    memset(statbuf, 0, sizeof(struct stat));
    statbuf->st_mode = S_IFDIR;
    return 0;
}

int devfs_interface_dir_close(struct kernel_dir_t* dir){
    free(dir);
    return 0;
}

struct kernel_dir_t* devfs_interface_dir_open(struct fs_t* ctx, const char* path, int* error){
    devfs_context_t* devfs_ctx = (devfs_context_t*)ctx->internal_data;
    devfs_directory_internal_t* devfs_dir = devfs_open_dir(devfs_ctx, devfs_interface_convert_path((char*)path), error);
    if(devfs_dir == NULL){
        return NULL;
    }
    kernel_dir_t* dir = malloc(sizeof(kernel_dir_t));
    dir->fs_ctx = ctx;
    dir->seek_position = 0;
    dir->internal_data = devfs_dir;
    dir->get_directory_entries = &devfs_interface_dir_get_directory_entries;
    dir->create_at = &devfs_interface_dir_create_at;
    dir->unlink_at = &devfs_interface_dir_unlink_at;
    dir->stat = &devfs_interface_dir_stat;
    dir->close = &devfs_interface_dir_close;

    return dir;
}


int devfs_interface_rename(struct fs_t* ctx, const char* old_path, const char* new_path){
    devfs_context_t* devfs_ctx = (devfs_context_t*)ctx->internal_data;
    int error = 0;
    devfs_directory_entry_t* entry = devfs_get_entry_with_path_from_root(devfs_ctx, old_path, &error);
    if(error){
        return error;
    }

    char* new_entry_name;
    devfs_directory_entry_t* new_parent = devfs_find_last_directory_with_path_from_root(devfs_ctx, new_path, &new_entry_name, &error);

    if(error){
        return error;
    }

    assert(!devfs_remove_entry(entry));

    assert(!devfs_change_entry_name(entry, new_entry_name));

    assert(!devfs_add_entry(new_parent, entry));

    return ENOSYS;
}

int devfs_interface_link(struct fs_t* ctx, const char* src_path, const char* dst_path){
    return ENOSYS;
}

int devfs_interface_stat(struct fs_t* ctx, const char* path, int flags, struct stat* statbuf){
    memset(statbuf, 0, sizeof(struct stat));
    statbuf->st_mode = S_IFIFO;
    return 0;
}

int devfs_add_dev(devfs_context_t* devfs_ctx, const char* path, file_open_fs_t open_handler){
    int error = 0;
    char* entry_name;
    devfs_directory_entry_t* parent = devfs_find_last_directory_with_path_from_root(devfs_ctx, path, &entry_name, &error);

    if(error){
        return error;
    }

    devfs_directory_entry_t* entry = malloc(sizeof(devfs_directory_entry_t));

    entry->data.file = malloc(sizeof(devfs_file_internal_t));

    entry->data.file->size = 0;

    entry->data.file->open_handler = open_handler;

    entry->parent = parent;

    entry->is_file = true;

    assert(!devfs_set_entry_name(entry, entry_name));

    assert(!devfs_add_entry(parent, entry));

    return 0;
}

devfs_context_t* init_devfs(void){
    devfs_context_t* ctx = malloc(sizeof(devfs_context_t));

    {
        ctx->root_entry = malloc(sizeof(devfs_directory_entry_t));

        ctx->root_entry->data.directory = devfs_creat_new_directory_field();

        ctx->root_entry->parent = NULL;

        ctx->root_entry->is_file = false;

        assert(!devfs_set_entry_name(ctx->root_entry, ""));
    }

    {
        fs_t* vfs_interface = malloc(sizeof(fs_t));
        vfs_interface->internal_data = ctx;
        vfs_interface->file_remove = &devfs_interface_file_remove;
        vfs_interface->file_open = &devfs_interface_file_open;
        vfs_interface->dir_create = &devfs_interface_dir_create;
        vfs_interface->dir_remove = &devfs_interface_dir_remove;
        vfs_interface->dir_open = &devfs_interface_dir_open;
        vfs_interface->rename = &devfs_interface_rename;
        vfs_interface->link = &devfs_interface_link;
        vfs_interface->stat = &devfs_interface_stat;

        assert(!vfs_mount_fs("/dev", vfs_interface));
        assert(!vfs_mount_fs("/devfs", vfs_interface));
    }

    return ctx;
}