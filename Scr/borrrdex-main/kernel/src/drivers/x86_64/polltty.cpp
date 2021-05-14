#include "drivers/polltty.h"
#include "drivers/device.h"
#include "drivers/gcd.h"
#include "drivers/x86_64/tty.h"
#include "drivers/x86_64/keyboard.h"

extern device_t* tty_init();

static tty_t* polltty_iobase;
static gcd_t* polltty_gcd;
device_t* tty_dev;

void polltty_init() {
    tty_dev = tty_init();
    polltty_iobase = (tty_t *)tty_dev->real_device;
    polltty_gcd = (gcd_t *)tty_dev->generic_device;

    for(int i = 0; i < tty::VIDEO_MEMORY_COLUMNS * tty::VIDEO_MEMORY_ROWS; i++) {
        polltty_putchar(' ');
    }

    polltty_iobase->cursor_x = polltty_iobase->cursor_y = 0;
}

int polltty_getchar() {
    return (int)keyboard_getkey();
}

void polltty_putchar(int c) {
    char ptr = (char) c;
    polltty_gcd->write(polltty_gcd, &ptr, 1);
}