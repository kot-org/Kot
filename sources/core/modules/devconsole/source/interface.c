#include <errno.h>
#include <lib/log.h>
#include <lib/math.h>
#include <global/heap.h>
#include <global/devfs.h>
#include <global/console.h>
#include <global/scheduler.h>

#define BUFFER_COUNT    2
#define KEY_BUFFER_SIZE 1024

spinlock_t key_handler_lock = {};

char key_buffer[BUFFER_COUNT][KEY_BUFFER_SIZE + 1];
uint8_t key_buffer_char_write_index = 0;
uint16_t key_buffer_char_index = 0;
uint16_t key_buffer_char_read_index[BUFFER_COUNT] = {0, 0};

thread_t* waiting_thread_for_input = NULL;
spinlock_t lock_waiting_thread_for_input = {};

void key_handler(uint64_t scancode, uint16_t translated_key, bool is_pressed){
    if(is_pressed){
        spinlock_acquire(&key_handler_lock);

        if((translated_key >= 0x21 && translated_key <= 0x7E) || translated_key == ' '){
            if(((key_buffer_char_index + 1)  % KEY_BUFFER_SIZE) == 0){
                log_warning("[module/"MODULE_NAME"] flushing key buffer\n");
            }

            key_buffer[key_buffer_char_write_index][key_buffer_char_index] = translated_key;
            key_buffer_char_index = (key_buffer_char_index + 1)  % KEY_BUFFER_SIZE;
            key_buffer[key_buffer_char_write_index][key_buffer_char_index] = '\0';

            console_putchar(translated_key);
        }else if(translated_key == '\n'){
            key_buffer[key_buffer_char_write_index][key_buffer_char_index] = translated_key;
            key_buffer_char_index = (key_buffer_char_index + 1) % KEY_BUFFER_SIZE;
            key_buffer[key_buffer_char_write_index][key_buffer_char_index] = '\0';
            
            console_putchar(translated_key);

            key_buffer_char_write_index = (key_buffer_char_write_index + 1) % BUFFER_COUNT;

            key_buffer_char_index = 0;
            key_buffer_char_read_index[key_buffer_char_write_index] = 0;
            key_buffer[key_buffer_char_write_index][key_buffer_char_index] = '\0';

            if(waiting_thread_for_input != NULL){
                scheduler_unpause_thread(waiting_thread_for_input);
            }
        }else if(translated_key == 0x8){
            if(key_buffer_char_index != 0){
                key_buffer_char_index--;
                key_buffer[key_buffer_char_write_index][key_buffer_char_index] = '\0';
                console_delchar();
            }
        }

        spinlock_release(&key_handler_lock);
    }
}

int console_interface_read(void* buffer, size_t size, size_t* bytes_read, struct kernel_file_t* file){
    if(spinlock_test_and_acq(&lock_waiting_thread_for_input)){
        uint8_t key_buffer_to_read_index = !key_buffer_char_write_index;

        if(key_buffer[key_buffer_to_read_index][key_buffer_char_read_index[key_buffer_to_read_index]] == '\0'){
            waiting_thread_for_input = scheduler_get_current_thread();
            scheduler_pause_thread(waiting_thread_for_input, NULL);
            waiting_thread_for_input = NULL;
            key_buffer_to_read_index = !key_buffer_char_write_index;
        }


        size_t size_read = 0;

        while(size_read < size && key_buffer[key_buffer_to_read_index][key_buffer_char_read_index[key_buffer_to_read_index]] != '\0'){
            ((char*)buffer)[size_read] = key_buffer[key_buffer_to_read_index][key_buffer_char_read_index[key_buffer_to_read_index]];
            key_buffer_char_read_index[key_buffer_to_read_index]++;
            size_read++;
        }

        *bytes_read = size_read;

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