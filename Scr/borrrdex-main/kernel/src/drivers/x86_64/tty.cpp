#include "tty.h"
#include "stalloc.h"
#include "Panic.h"
#include "spinlock.h"
#include "drivers/device.h"
#include "drivers/gcd.h"
#include "drivers/x86_64/keyboard.h"
#include "arch/x86_64/io/io.h"
#include "arch/x86_64/interrupt/interrupt.h"

static int tty_write(gcd_t* gcd, const void* buf, int len) {
    uint64_t vidbase = (uint64_t) gcd->device->io_address;
    uint8_t *str = (uint8_t*) buf;
    tty_t *tty = (tty_t*) gcd->device->real_device;
    int j = len;

    WithInterrupts wi(false);
    tty->slock->acquire();

    while(j) {
        uint8_t c = (uint8_t) *str;
        if(c == 0) {
            break;
        }

        uint8_t fcolor = (uint8_t)tty->text_color;
        uint16_t t_attrib = (uint16_t)(fcolor << 8);

        uint64_t offset = (tty->cursor_y * tty::VIDEO_MEMORY_COLUMNS +
                         tty->cursor_x) * 2;
        volatile uint16_t* vptr = (volatile uint16_t *)(vidbase + offset);
        switch(c) {
            case 0x08:
                if(tty->cursor_x != 0) {
                    tty->cursor_x--;
                    vptr--;
                    *vptr = (' ' | t_attrib);
                }

                break;
            case '\r':
                tty->cursor_x = 0;
                break;
            case '\n':
                tty->cursor_y++;
                tty->cursor_x = 0;
                break;
            default:
                *vptr = (t_attrib | c);
                tty->cursor_x++;
                break;
        }

        if(tty->cursor_x >= tty::VIDEO_MEMORY_COLUMNS) {
            tty->cursor_x = 0;
            tty->cursor_y++;
        }

        if(tty->cursor_y >= tty::VIDEO_MEMORY_ROWS) {
            uint16_t *sptr = (uint16_t*) vidbase;
            for(int i = 0; i < (tty::VIDEO_MEMORY_ROWS-1) * tty::VIDEO_MEMORY_COLUMNS; i++) {
                sptr[i] = sptr[i + tty::VIDEO_MEMORY_COLUMNS];
            }

            for(int i = (tty::VIDEO_MEMORY_ROWS-1) * tty::VIDEO_MEMORY_COLUMNS; 
                    i < tty::VIDEO_MEMORY_ROWS * tty::VIDEO_MEMORY_COLUMNS; i++) {
                sptr[i] = (uint16_t)(' ' | t_attrib);
            }

            tty->cursor_y--;
            tty->cursor_x = 0;
        }

        str++;
        j--;
    }

    // uint16_t cursor_loc = tty->cursor_y * tty::VIDEO_MEMORY_COLUMNS + tty->cursor_x;
    // port_write_8(0x3D4, 14);
    // port_write_8(0x3D5, (uint8_t)cursor_loc >> 8);
    // port_write_8(0x3D4, 15);
    // port_write_8(0x3D5, (uint8_t)cursor_loc);

    tty->slock->release();
    
    return len;
}

static int tty_read(gcd_t* gcd, void* buffer, int len) {
    int i = 0, j = len;
    char *str = (char*) buffer;
    while(j) {
        char key = keyboard_getkey();
        if(key == '\n') {
            break;
        }

        *str = key;
        i++;
        str++;
        j--;
    }

    return i;
}

device_t* tty_init() {
    device_t* dev = (device_t *)stalloc(sizeof(device_t));
    if(!dev) {
        Panic("Could not reserve memory for tty driver.");
    }

    gcd_t* gcd = (gcd_t *)stalloc(sizeof(gcd_t));
    if(!gcd) {
        Panic("Could not reserve memory for tty driver.");
    }

    dev->generic_device = gcd;
    dev->io_address = tty::VIDEO_MEMORY_BASE;
    dev->type = TYPECODE_TTY;

    gcd->device = dev;
    gcd->read = tty_read;
    gcd->write = tty_write;

    tty_t* tty = (tty_t *)stalloc(sizeof(tty_t));
    if(!tty) {
        Panic("Could not reserve memory for tty driver.");
    }

    tty->cursor_x = 0;
    tty->cursor_y = 0;
    tty->text_color = tty::COLOR_WHITE;

    dev->real_device = tty;

    tty->slock = (Spinlock *)stalloc(sizeof(Spinlock));
    tty->slock->reset();

    return dev;
}