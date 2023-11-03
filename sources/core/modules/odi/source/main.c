#include <lib/log.h>
#include <global/modules.h>

#include <deps.c>

#include "../odi/odi-ocf.c"

#define MODULE_NAME "odi"

int init(int argc, char* args[]){
    log_printf("[module/"MODULE_NAME"] loading start\n");

    fs_t* vfs_interface = malloc(sizeof(fs_t));
	vfs_interface->internal_data = ctx;
    vfs_interface->file_remove = &fat_interface_file_remove;
    vfs_interface->file_open = &fat_interface_file_open;
    vfs_interface->dir_create = &fat_interface_dir_create;
    vfs_interface->dir_remove = &fat_interface_dir_remove;
    vfs_interface->dir_open = &fat_interface_dir_open;
    vfs_interface->rename = &fat_interface_rename;
    vfs_interface->link = &fat_interface_link;

    assert(!vfs_mount_fs("/odi", vfs_interface));

    log_printf("[module/"MODULE_NAME"] loading end\n");

    return 0;
}

int fini(void){
    return 0;
}

module_metadata_t module_metadata = {
    &init,
    &fini,
    MODULE_TYPE_UNDEFINE,
    MODULE_NAME
};
