#include <errno.h>
#include <lib/log.h>
#include <global/heap.h>
#include <global/devfs.h>
#include <global/console.h>
#include <global/scheduler.h>

#define KEY_BUFFER_SIZE 1024

spinlock_t key_handler_lock = {};

char key_buffer[KEY_BUFFER_SIZE + 1];
uint16_t key_buffer_index = 0;

thread_t* waiting_thread_for_input = NULL;
spinlock_t lock_waiting_thread_for_input = {};

void key_handler(uint64_t scancode, uint16_t translated_key, bool is_pressed){
    if(is_pressed){
        spinlock_acquire(&key_handler_lock);

        if((translated_key >= 0x21 && translated_key <= 0x7E) || translated_key == ' '){
            if(((key_buffer_index + 1)  % (KEY_BUFFER_SIZE - 1)) == 0){
                log_warning("[module/"MODULE_NAME"] flushing key buffer\n");
            }

            key_buffer[key_buffer_index] = translated_key;
            key_buffer_index = (key_buffer_index + 1)  % (KEY_BUFFER_SIZE - 1);
            key_buffer[key_buffer_index] = '\0';

            console_putchar(translated_key);
        }else if(translated_key == '\n'){
            console_putchar(translated_key);
            
            log_success("sending '%s' to the console\n", key_buffer);

            if(!spinlock_test_and_acq(&lock_waiting_thread_for_input)){
                log_success("1\n");
                scheduler_unpause_thread(waiting_thread_for_input);
                log_success("2\n");
                /* wait the main thread before overwritting the key buffer*/
                log_success("3\n");
                spinlock_acquire(&lock_waiting_thread_for_input);
                log_success("4\n");
                key_buffer_index = 0;
                log_success("5\n");
                key_buffer[key_buffer_index] = '\0';
                spinlock_release(&lock_waiting_thread_for_input);
            }else{
                log_warning("No one are watching what you ae doing ! \n");
                spinlock_release(&lock_waiting_thread_for_input);
            }


        }else if(translated_key == 0x8){
            if(key_buffer_index != 0){
                key_buffer_index--;
                key_buffer[key_buffer_index] = '\0';
                console_delchar();
            }
        }

        spinlock_release(&key_handler_lock);
    }
}

int console_interface_read(void* buffer, size_t size, size_t* bytes_read, struct kernel_file_t* file){
    if(spinlock_test_and_acq(&lock_waiting_thread_for_input)){
        log_success("trying to read input\n");
        waiting_thread_for_input = scheduler_get_current_thread();
        log_success("0\n");
        scheduler_pause_thread(waiting_thread_for_input, NULL);
        log_success("-1\n");
        spinlock_release(&lock_waiting_thread_for_input);
        return 0;
    }else{
        *bytes_read = 0;
        return EIO;
    }
}

int console_interface_write(void* buffer, size_t size, size_t* bytes_write, kernel_file_t* file){
    log_printf("%.*s", size, buffer);
    *bytes_write = size;
    return 0;
}

int console_interface_seek(off_t offset, int whence, off_t* new_offset, kernel_file_t* file){
    *new_offset = 0;
    return 0;
}

int console_interface_ioctl(uint32_t request, void* arg, int* result, kernel_file_t* file){
    return ENOSYS;
}

int console_interface_stat(int flags, struct stat* statbuf, kernel_file_t* file){
    return ENOSYS;
}

int console_interface_close(kernel_file_t* file){
    free(file);
    return 0;
}

kernel_file_t* console_interface_open(struct fs_t* ctx, const char* path, int flags, mode_t mode, int* error){
    kernel_file_t* file = malloc(sizeof(kernel_file_t));

    file->fs_ctx = ctx;
    file->seek_position = 0;
    file->file_size_initial = 0;
    file->internal_data = NULL;

    file->read = console_interface_read;
    file->write = console_interface_write;
    file->seek = console_interface_seek;
    file->ioctl = console_interface_ioctl;
    file->stat = console_interface_stat;
    file->close = console_interface_close;

    return file;
}

void interface_init(void){
    hid_handler->set_key_handler(&key_handler);

    devfs_add_dev("tty0", &console_interface_open);
}