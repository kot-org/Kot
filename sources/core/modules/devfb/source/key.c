#include <errno.h>
#include <lib/log.h>
#include <lib/math.h>
#include <sys/ioctl.h>
#include <global/heap.h>
#include <global/devfs.h>
#include <global/console.h>
#include <global/scheduler.h>

uint64_t last_scancode;
bool is_last_scancode = false;
spinlock_t key_handler_lock = SPINLOCK_INIT;

void key_handler(uint64_t scancode, uint16_t translated_key, bool is_pressed){
    spinlock_acquire(&key_handler_lock);

    last_scancode = scancode | ((uint64_t)is_pressed << 63);
    is_last_scancode = true;

    spinlock_release(&key_handler_lock);
}

int fb_interface_read(void* buffer, size_t size, size_t* bytes_read, struct kernel_file_t* file){
    if(spinlock_test_and_acq(&key_handler_lock)){
        size_t size_read = 0;

        if(is_last_scancode){
            *((uint64_t*)buffer) = last_scancode;
            is_last_scancode = false;

            *bytes_read = 1;
        }else{
            *bytes_read = 0;
        }

        spinlock_release(&key_handler_lock);
        return 0;
    }else{
        *bytes_read = 0;
        return EIO;
    }
}