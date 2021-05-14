#include "drivers/device.h"
#include "config.h"
#include "pit.h"
#include "arch/x86_64/interrupt/interrupt.h"
#include "drivers/x86_64/keyboard.h"

#include <stdatomic.h>

static device_t *s_device_table[CONFIG_MAX_DEVICES];

static int s_number_of_devices;

//extern device_t *tty_dev;

void device_init() {
    //s_device_table[s_number_of_devices++] = tty_dev;

    pit_init();

    interrupt_enable();

    keyboard_init();
}

device_t* device_get(uint32_t typecode, uint32_t n) {
    for(int i = 0; i < s_number_of_devices; i++) {
        if(s_device_table[i]->type == typecode) {
            if(n-- == 0) {
                return s_device_table[i];
            }
        }
    }

    return nullptr;
}

int device_register(device_t* device) {
    if(s_number_of_devices >= CONFIG_MAX_DEVICES) {
        return -1;
    }

    int index = atomic_increment_i32(&s_number_of_devices);
    s_device_table[index] = device;
    return 0;
}