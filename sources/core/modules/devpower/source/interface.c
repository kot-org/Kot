#include <poll.h>
#include <errno.h>
#include <lib/log.h>
#include <lib/lock.h>
#include <impl/vmm.h>
#include <linux/fb.h>
#include <lib/string.h>
#include <global/pmm.h>
#include <global/heap.h>
#include <global/devfs.h>
#include <impl/graphics.h>

#include <arch/include.h>
#include ARCH_INCLUDE(impl/arch.h)

int power_interface_write(void* buffer, size_t size, size_t* bytes_write, kernel_file_t* file){
    *bytes_write = size;
    return 0;
}

int power_interface_read(void* buffer, size_t size, size_t* bytes_read, struct kernel_file_t* file){
    *bytes_read = size;
    return 0;
}

int power_interface_seek(off_t offset, int whence, off_t* new_offset, kernel_file_t* file){
    return 0;
}

int power_interface_ioctl(uint32_t request, void* arg, int* result, kernel_file_t* file){
    switch(request){
        case 0:{
            arch_shutdown();
            break;
        }
        case 1:{
            arch_reboot();
            break;
        }
        default:{
            return EINVAL;
        }
    }

    return 0;
}

int power_interface_stat(int flags, struct stat* statbuf, kernel_file_t* file){
    memset(statbuf, 0, sizeof(struct stat));
    statbuf->st_mode = S_IFIFO;
    return 0;
}

int power_interface_close(kernel_file_t* file){
    free(file);
    return 0;
}

int power_interface_get_event(kernel_file_t* file, short event, short* revent){
    *revent = (event & (POLLIN | POLLOUT));

    int event_count = 0;
    if(event & POLLIN){
        event_count++;
    }
    if(event & POLLOUT){
        event_count++;
    }
    
    return event_count;
}

kernel_file_t* power_interface_open(struct fs_t* ctx, const char* path, int flags, mode_t mode, int* error){
    kernel_file_t* file = malloc(sizeof(kernel_file_t));

    file->fs_ctx = ctx;
    file->seek_position = 0;
    file->file_size_initial = 0;
    file->internal_data = NULL;

    file->read = power_interface_read;
    file->write = power_interface_write;
    file->seek = power_interface_seek;
    file->ioctl = power_interface_ioctl;
    file->stat = power_interface_stat;
    file->close = power_interface_close;
    file->get_event = power_interface_get_event;

    return file;
}


void interface_init(void){
    devfs_add_dev("power", &power_interface_open);
}