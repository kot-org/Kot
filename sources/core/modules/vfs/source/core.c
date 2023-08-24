#include <core.h>

static hashmap_t* vfs_hashmap;

void init_vfs(void){
    vfs_hashmap = hashmap_create(32);
}

fs_t* get_fs(const char* fs_mount_name){
	return (fs_t*)hashmap_get(vfs_hashmap, fs_mount_name);
}

int add_fs(const char* fs_mount_name, fs_t* fs){
	hashmap_set(vfs_hashmap, fs_mount_name, fs);
    return 0;
}

int remove_fs(const char* fs_mount_name){
	hashmap_remove(vfs_hashmap, fs_mount_name);
    return 0;
}