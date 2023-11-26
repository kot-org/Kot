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

#define FB_CLEAR_COLOR 0x00000000

static bool use_boot_fb = false;
static spinlock_t boot_fb_lock = SPINLOCK_INIT;

static graphics_boot_fb_t* boot_fb = NULL;
static struct fb_fix_screeninfo fix_screeninfo = {};
static struct fb_var_screeninfo var_screeninfo = {};

static bool fb_need_to_be_request = true;
static int x_fb_start = 0;
static int y_fb_start = 0;


static int boot_fb_callback(void){
    spinlock_acquire(&boot_fb_lock);
    fb_need_to_be_request = true;
    spinlock_release(&boot_fb_lock);
    return 0;
}

static void clear_fb(void){
    if(boot_fb != NULL){
        memset32(boot_fb->base, FB_CLEAR_COLOR, boot_fb->size);
    }
}

static void request_fb(void){
    spinlock_acquire(&boot_fb_lock);
    if(fb_need_to_be_request){
        /* get key handler */
        hid_handler->set_key_handler(&key_handler);
        
        boot_fb = graphics_get_boot_fb(&boot_fb_callback);
        clear_fb();
        fb_need_to_be_request = false;
    }
}

static void release_fb(void){
    spinlock_release(&boot_fb_lock);
}

static void init_boot_fb(void){
    request_fb();

    /* fill fix_screeninfo struct */
    strcpy((char*)&fix_screeninfo.id, "BOOT FB");
    fix_screeninfo.smem_start = (unsigned long)vmm_get_physical_address(vmm_get_kernel_space(), boot_fb->base);
    fix_screeninfo.smem_len = boot_fb->size;
    fix_screeninfo.type = FB_TYPE_PACKED_PIXELS;
    fix_screeninfo.type_aux = 0;
    fix_screeninfo.visual = FB_VISUAL_TRUECOLOR;
    fix_screeninfo.xpanstep = 0;
    fix_screeninfo.ywrapstep = 0;
    fix_screeninfo.line_length = boot_fb->pitch;
    fix_screeninfo.mmio_start = (unsigned long)boot_fb->base; // memory mapped address are not always physical
    fix_screeninfo.mmio_len = boot_fb->size + ((boot_fb->size % PAGE_SIZE) ? PAGE_SIZE - boot_fb->size % PAGE_SIZE : 0); // make it page aligned
    fix_screeninfo.accel = 0;
    fix_screeninfo.capabilities = 0;

    /* fill var_screeninfo struct */
    var_screeninfo.xres = boot_fb->width;
    var_screeninfo.yres = boot_fb->height;
    var_screeninfo.xres_virtual = var_screeninfo.xres;
    var_screeninfo.yres_virtual = var_screeninfo.yres;
    var_screeninfo.xoffset = 0;
    var_screeninfo.yoffset = 0;

    var_screeninfo.bits_per_pixel = boot_fb->bpp;
    var_screeninfo.grayscale = 0;

    var_screeninfo.red = (struct fb_bitfield){16, 8, 0};
    var_screeninfo.green = (struct fb_bitfield){8, 8, 0};
    var_screeninfo.blue = (struct fb_bitfield){0, 8, 0};

    var_screeninfo.nonstd = 0;

    var_screeninfo.activate = FB_ACTIVATE_NOW;

    var_screeninfo.height = boot_fb->width / 3; // let's say we have 3 pixels per mm
    var_screeninfo.width = boot_fb->height / 3; // let's say we have 3 pixels per mm

    var_screeninfo.accel_flags = 0;

    var_screeninfo.pixclock = 0;
    var_screeninfo.left_margin = 0;
    var_screeninfo.right_margin = 0;
    var_screeninfo.upper_margin = 0;
    var_screeninfo.lower_margin = 0;
    var_screeninfo.hsync_len = 0;
    var_screeninfo.vsync_len = 0;
    var_screeninfo.sync = FB_SYNC_EXT;
    var_screeninfo.vmode = FB_VMODE_NONINTERLACED;
    var_screeninfo.rotate = 0;
    var_screeninfo.colorspace = 0;

    release_fb();
}

int fb_interface_write(void* buffer, size_t size, size_t* bytes_write, kernel_file_t* file){
    request_fb();

    if(boot_fb == NULL){
        return EACCES;
    }

    ssize_t size_to_write = size;
    if(size_to_write > boot_fb->size){
        size_to_write = boot_fb->size;
    }

    if(size_to_write > var_screeninfo.yres_virtual * var_screeninfo.xres_virtual * boot_fb->bpp){
        size_to_write = var_screeninfo.yres_virtual * var_screeninfo.xres_virtual * boot_fb->bpp;
    }

    uintptr_t offset_write = (uintptr_t)boot_fb->base + (uintptr_t)y_fb_start * (uintptr_t)boot_fb->pitch;
    uintptr_t offset_read = (uintptr_t)buffer;

    while(size_to_write != 0){
        size_t size_to_write_line = size_to_write;
        if(size_to_write_line > boot_fb->pitch){
            size_to_write_line = boot_fb->pitch;
        }

        if(size_to_write_line > var_screeninfo.xres_virtual * boot_fb->btpp){
            size_to_write_line = var_screeninfo.xres_virtual * boot_fb->btpp;
        }

        memcpy((void*)((uintptr_t)offset_write  + (uintptr_t)x_fb_start * (uintptr_t)boot_fb->btpp), (void*)offset_read, size_to_write_line);

        offset_write += boot_fb->pitch;
        offset_read += size_to_write_line;
        size_to_write -= size_to_write_line;
    }

    release_fb();

    *bytes_write = size;
    return 0;
}

int fb_interface_seek(off_t offset, int whence, off_t* new_offset, kernel_file_t* file){
    return 0;
}

int fb_interface_ioctl(uint32_t request, void* arg, int* result, kernel_file_t* file){
    switch (request){
        case FBIOGET_FSCREENINFO:{
            if(vmm_check_memory(vmm_get_current_space(), (memory_range_t){arg, sizeof(struct fb_fix_screeninfo)})){
                return EINVAL;
            }
            memcpy(arg, &fix_screeninfo, sizeof(struct fb_fix_screeninfo));
            *result = 0;
            return 0;        
        }
        case FBIOGET_VSCREENINFO:{
            if(vmm_check_memory(vmm_get_current_space(), (memory_range_t){arg, sizeof(struct fb_var_screeninfo)})){
                return EINVAL;
            }
            memcpy(arg, &var_screeninfo, sizeof(struct fb_var_screeninfo));
            *result = 0;
            return 0;        
        }
        case FBIOPUT_VSCREENINFO:{
            if(vmm_check_memory(vmm_get_current_space(), (memory_range_t){arg, sizeof(struct fb_var_screeninfo)})){
                return EINVAL;
            }

            if(((struct fb_var_screeninfo*)arg)->xres_virtual <= boot_fb->width && ((struct fb_var_screeninfo*)arg)->yres_virtual <= boot_fb->height){
                var_screeninfo.xres_virtual = ((struct fb_var_screeninfo*)arg)->xres_virtual;
                var_screeninfo.yres_virtual = ((struct fb_var_screeninfo*)arg)->yres_virtual;

                x_fb_start = (boot_fb->width - var_screeninfo.xres_virtual) / 2; 
                y_fb_start = (boot_fb->height - var_screeninfo.yres_virtual) / 2; 
            }
            *result = 0;
            return 0;
        }
        default:{
            return EINVAL;
        }
    }
}

int fb_interface_stat(int flags, struct stat* statbuf, kernel_file_t* file){
    return ENOSYS;
}

int fb_interface_close(kernel_file_t* file){
    free(file);
    return 0;
}

kernel_file_t* fb_interface_open(struct fs_t* ctx, const char* path, int flags, mode_t mode, int* error){
    /* if we don't have the boot fb yet request it */
    if(boot_fb == NULL){
        init_boot_fb();
    }

    kernel_file_t* file = malloc(sizeof(kernel_file_t));

    file->fs_ctx = ctx;
    file->seek_position = 0;
    file->file_size_initial = 0;
    file->internal_data = NULL;

    file->read = fb_interface_read;
    file->write = fb_interface_write;
    file->seek = fb_interface_seek;
    file->ioctl = fb_interface_ioctl;
    file->stat = fb_interface_stat;
    file->close = fb_interface_close;

    return file;
}


void interface_init(void){
    devfs_add_dev("fb0", &fb_interface_open);
}