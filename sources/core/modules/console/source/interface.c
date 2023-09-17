#include <errno.h>
#include <lib/log.h>
#include <global/devfs.h>

int console_interface_read(void* buffer, size_t size, size_t* bytes_read, struct kernel_file_t* file){
    *bytes_read = 0;
    return ENOSYS;
}

int console_interface_write(void* buffer, size_t size, size_t* bytes_write, kernel_file_t* file){
    log_printf("%.*s", size, buffer);
    *bytes_write = size;
    return 0;
}

int console_interface_seek(off_t offset, int whence, off_t* new_offset, kernel_file_t* file){
    return 0;
}

int console_interface_ioctl(uint32_t request, void* arg, int* result, kernel_file_t* file){
    return ENOSYS;
}

int console_interface_stat(int flags, struct stat* statbuf, kernel_file_t* file){
    return ENOSYS;
}

int console_interface_close(kernel_file_t* file){
    return 0;
}

void interface_init(void){
    devfs_functions_t console_functions;

    console_functions.read = console_interface_read;
    console_functions.write = console_interface_write;
    console_functions.seek = console_interface_seek;
    console_functions.ioctl = console_interface_ioctl;
    console_functions.stat = console_interface_stat;
    console_functions.close = console_interface_close;

    devfs_add_dev("tty0", &console_functions);
}