#ifndef LIB_MODULES_HID_H
#define LIB_MODULES_HID_H 1

#include <stdint.h>

typedef void (*key_handler_t)(uint64_t, uint16_t, bool);

typedef struct{
    int (*key_update_state)(uint64_t, bool);
    int (*set_key_handler)(key_handler_t);
    int (*update_scancode_translation_table)(uint16_t* table, size_t size);
} hid_handler_t;

#endif // LIB_MODULES_HID_H