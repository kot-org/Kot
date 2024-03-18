#define SUB_TABLE_DEFAULT_INDEX                 0x0
#define SUB_TABLE_SHIFT_INDEX                   0x1
#define SUB_TABLE_ALT_GR_INDEX                  0x2
#define SUB_TABLE_SHIFT_AND_ALT_GR_INDEX        0x3
#define SUB_TABLE_COUNT                         0x4

#define TSK_SHIFT       0xF
#define TSK_CAPS_LOCK   0x11
#define TSK_ALT_GR      0x13

hid_handler_t hid_internal_handler;

key_handler_t handler;
spinlock_t scancode_translation_table_lock = SPINLOCK_INIT;
uint16_t* scancode_table_buffer = NULL;
size_t scancode_table_size = 0;

uint16_t translated_key_shift_start = 0;
uint16_t translated_key_alt_gr_start = 0;

void key_handler_stub(uint64_t scancode, uint16_t translated_key, bool is_pressed){
    #ifdef DEBUG_KEYBOARD
    if(translated_key >= 0x21 && translated_key <= 0x7E){
        log_warning("%s : stub, scancode : %d, char : %c, key %s\n", __func__, scancode, translated_key, is_pressed ? "pressed" : "release");
    }else if(translated_key){
        log_warning("%s : stub, scancode : %d, translated code : %d, key %s\n", __func__, scancode, translated_key, is_pressed ? "pressed" : "release");
    }
    #endif
}

int key_update_state(uint64_t scancode, bool is_pressed){
    static bool is_caps_lock = false;
    static bool is_shift_pressed = false;
    static bool is_alt_gr_pressed = false;

    uint16_t translated_key = 0;

    assert(!spinlock_acquire(&scancode_translation_table_lock));

    uint64_t translation_index = scancode;

    if(is_caps_lock ^ is_shift_pressed){
        translation_index = translation_index + translated_key_shift_start;
    }
    
    if(is_alt_gr_pressed){
        translation_index = translation_index + translated_key_alt_gr_start;
    }

    if(translation_index < scancode_table_size){
        translated_key = scancode_table_buffer[translation_index];
    }

    spinlock_release(&scancode_translation_table_lock);

    if(translated_key == TSK_CAPS_LOCK && is_pressed){
        is_caps_lock = !is_caps_lock;
    }else if(translated_key == TSK_SHIFT){
        is_shift_pressed = is_pressed;
    }else if(translated_key == TSK_ALT_GR){
        is_alt_gr_pressed = is_pressed;
    }

    handler(scancode, translated_key, is_pressed);

    return 0;
}

int set_key_handler(key_handler_t handler_to_set){
    handler = handler_to_set;
    return 0;
}

int update_scancode_translation_table(uint16_t* table, size_t size){
    assert(!spinlock_acquire(&scancode_translation_table_lock));
    scancode_table_buffer = table;
    scancode_table_size = size;
    translated_key_shift_start = size * SUB_TABLE_SHIFT_INDEX / SUB_TABLE_COUNT;
    translated_key_alt_gr_start = size * SUB_TABLE_ALT_GR_INDEX / SUB_TABLE_COUNT;
    spinlock_release(&scancode_translation_table_lock);
    return 0;
}

void interface_init(void){
    set_key_handler(&key_handler_stub);

    hid_internal_handler.key_update_state = &key_update_state;
    hid_internal_handler.set_key_handler = &set_key_handler;
    hid_internal_handler.update_scancode_translation_table = &update_scancode_translation_table;
    hid_handler = &hid_internal_handler;
}