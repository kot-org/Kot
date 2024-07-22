#include <poll.h>
#include <errno.h>
#include <signal.h>
#include <termios.h>
#include <lib/log.h>
#include <lib/math.h>
#include <sys/ioctl.h>
#include <global/heap.h>
#include <global/devfs.h>
#include <global/console.h>
#include <global/scheduler.h>

#include <console/console.c>

#define BUFFER_COUNT    2
#define KEY_BUFFER_SIZE 1024

struct termios console_termios = (struct termios){
    0, 
    0, 
    0, 
    0, 
    0, 
    {
        4,    // VEOF
        '\n', // VEOL
        '\b', // VERASE
        0,    // VINTR
        0,    // VKILL
        0,    // VMIN
        0,    // VQUIT
        0,    // VSTART
        0,    // VSTOP
        0,    // VSUSP
        0,    // VTIME
    },
    0,
    0
};
struct winsize console_window_size = {};

spinlock_t key_handler_lock = SPINLOCK_INIT;

char key_buffer[KEY_BUFFER_SIZE + 1];
uint16_t key_buffer_char_index = 0;
size_t key_buffer_size = 0;

thread_t* waiting_thread_for_input = NULL;
spinlock_t lock_waiting_thread_for_input = {};

bool crtl_key_is_pressed = false;

static void add_key_to_key_buffer(uint16_t key){
    if(key_buffer_size < KEY_BUFFER_SIZE){
        key_buffer[key_buffer_char_index] = key;
        key_buffer_char_index = (key_buffer_char_index + 1);
        key_buffer[key_buffer_char_index] = '\0';

        key_buffer_size++;

        if(waiting_thread_for_input != NULL){
            thread_t* thread_to_start = waiting_thread_for_input;
            waiting_thread_for_input = NULL;
            scheduler_unpause_thread(thread_to_start);
        }
    }
}

void print_to_key_buffer(char* str){
    while(*str){
        add_key_to_key_buffer((uint16_t)*str);
        str++;
    }
}

void key_handler(uint64_t scancode, uint16_t translated_key, bool is_pressed){
    if(translated_key == 0x12){
        assert(!spinlock_acquire(&key_handler_lock));
        crtl_key_is_pressed = is_pressed;
        spinlock_release(&key_handler_lock);
    }else if(is_pressed){
        assert(!spinlock_acquire(&key_handler_lock));

        if(devconsole_isprintable(translated_key)){
            if(crtl_key_is_pressed){
                if(translated_key == 'c'){
                    if(last_thread_output != NULL){
                        scheduler_signal_thread(last_thread_output, SIGINT);
                        last_thread_output = NULL;
                        waiting_thread_for_input = NULL;
                        spinlock_release(&lock_waiting_thread_for_input);
                    }
                }
            }else{
                add_key_to_key_buffer(translated_key);
                if(is_cursor){
                    devconsole_putchar(translated_key);
                }
            }
        }else if(translated_key == '\n'){
            add_key_to_key_buffer(translated_key);
            
            if(is_cursor){
                devconsole_putchar(translated_key);
            }
        }else if(translated_key == '\b'){
            add_key_to_key_buffer(translated_key);
            if(is_cursor){
                devconsole_delchar();
            }
        }

        cursor_update();

        spinlock_release(&key_handler_lock);
    }
}

int console_interface_read(void* buffer, size_t size, size_t* bytes_read, struct kernel_file_t* file){
    if(spinlock_test_and_acq(&lock_waiting_thread_for_input)){
        last_thread_output = scheduler_get_current_thread();

        if(key_buffer_size == 0){
            waiting_thread_for_input = scheduler_get_current_thread();
            scheduler_pause_thread(waiting_thread_for_input, NULL);
        }


        size_t size_read = 0;

        while(size_read < key_buffer_size){
            ((char*)buffer)[size_read] = key_buffer[size_read];
            size_read++;
        }

        key_buffer_size = 0;
        key_buffer_char_index = 0;

        *bytes_read = size_read;

        spinlock_release(&lock_waiting_thread_for_input);
        return 0;
    }else{
        *bytes_read = 0;
        return EIO;
    }
}

int console_interface_write(void* buffer, size_t size, size_t* bytes_write, kernel_file_t* file){
    devconsole_request_fb();
    
    console_window_size.ws_col = cx_max_index;
    console_window_size.ws_row = cy_max_index;
    console_window_size.ws_xpixel = fb_width;
    console_window_size.ws_ypixel = fb_height;

    assert(!spinlock_acquire(&key_handler_lock));
    last_thread_output = scheduler_get_current_thread();
    spinlock_release(&key_handler_lock);

    devconsole_print(buffer, size);
    *bytes_write = size;
    return 0;
}

int console_interface_seek(off_t offset, int whence, off_t* new_offset, kernel_file_t* file){
    *new_offset = 0;
    return 0;
}

int console_interface_ioctl(uint32_t request, void* arg, int* result, kernel_file_t* file){
    switch(request){
        case TCGETS:{
            if(vmm_check_memory(vmm_get_current_space(), (memory_range_t){arg, sizeof(struct termios)})){
                return EINVAL;
            }
            memcpy(arg, &console_termios, sizeof(struct termios)); 
            *result = 0;
            return 0;
        }
        case TCSETS:{
            if(vmm_check_memory(vmm_get_current_space(), (memory_range_t){arg, sizeof(struct termios)})){
                return EINVAL;
            }
            memcpy(&console_termios, arg, sizeof(struct termios)); 
            *result = 0;
            return 0;
        }
        case TIOCGWINSZ:{
            if(vmm_check_memory(vmm_get_current_space(), (memory_range_t){arg, sizeof(struct winsize)})){
                return EINVAL;
            }
            memcpy(arg, &console_window_size, sizeof(struct winsize)); 
            *result = 0;
            return 0;
        }
        case TIOCSWINSZ:{
            *result = 0;
            return 0;
        }
    }
    return EINVAL;
}

int console_interface_stat(int flags, struct stat* statbuf, kernel_file_t* file){
    memset(statbuf, 0, sizeof(struct stat));
    statbuf->st_mode = S_IFIFO;
    return 0;
}

int console_interface_close(kernel_file_t* file){
    assert(!spinlock_acquire(&key_handler_lock));
    last_thread_output = NULL;
    spinlock_release(&key_handler_lock);

    free(file);
    return 0;
}

int console_interface_get_event(kernel_file_t* file, short event, short* revent){
    *revent = (event & POLLOUT);

    int event_count = 0;

    if(event & POLLOUT){
        event_count++;
    }
    
    if(event & POLLIN){
        if(key_buffer_size){
            *revent |= POLLIN;
            event_count++;
        }
    }

    return event_count;
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
    file->get_event = console_interface_get_event;

    return file;
}

void interface_init(void){
    hid_handler->set_key_handler(&key_handler);

    devfs_add_dev("tty", &console_interface_open);

    devconsole_init();
}