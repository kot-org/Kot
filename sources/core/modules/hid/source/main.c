#include <lib/log.h>
#include <lib/lock.h>
#include <lib/assert.h>
#include <global/heap.h>
#include <global/file.h>
#include <global/modules.h>

#define SCANCODE_DEFAULT_FILE_PATH "/initrd/system/default/scancode.scd"

#define MODULE_NAME "hid"

#define DEBUG_KEYBOARD

#include <interface.c>

int init(int argc, char* args[]){
    log_printf("[module/"MODULE_NAME"] loading start\n");

    assert(modules_request_dependency(MODULE_TYPE_VFS) == 0);

    int error;

    kernel_file_t* scancode_file = f_open(KERNEL_VFS_CTX, SCANCODE_DEFAULT_FILE_PATH, 0, 0, &error);

    if(scancode_file != NULL){
        uint64_t bytes_read = 0;

        uint16_t* default_table = malloc(scancode_file->file_size_initial);

        assert(!f_read(default_table, scancode_file->file_size_initial, &bytes_read, scancode_file));

        assert(bytes_read == scancode_file->file_size_initial);

        assert(!f_close(scancode_file));

        size_t default_table_size = scancode_file->file_size_initial / sizeof(uint16_t);

        update_scancode_translation_table(default_table, default_table_size);

        log_success("[module/"MODULE_NAME"] '"SCANCODE_DEFAULT_FILE_PATH"' found\n");
    }else{
        log_warning("[module/"MODULE_NAME"] openning of '"SCANCODE_DEFAULT_FILE_PATH"', failed with error code : %d\n", error);
    }

    interface_init();

    log_printf("[module/"MODULE_NAME"] loading end\n");
    return 0;
}

int fini(void){
    return 0;
}

module_metadata_t module_metadata = {
    &init,
    &fini,
    MODULE_TYPE_HID,
    MODULE_NAME
};
