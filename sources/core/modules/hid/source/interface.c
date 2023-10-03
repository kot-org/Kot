hid_handler_t hid_internal_handler;

key_handler_t handler;
spinlock_t scancode_translation_table_lock = {};
uint16_t* scancode_table_buffer = NULL;
size_t scancode_table_size = 0;

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
    uint16_t translated_key = 0;

    spinlock_acquire(&scancode_translation_table_lock);

    if(scancode < scancode_table_size){
        translated_key = scancode_table_buffer[scancode];
    }

    spinlock_release(&scancode_translation_table_lock);

    handler(scancode, translated_key, is_pressed);

    return 0;
}

int set_key_handler(key_handler_t handler_to_set){
    handler = handler_to_set;
    return 0;
}

int update_scancode_translation_table(uint16_t* table, size_t size){
    spinlock_acquire(&scancode_translation_table_lock);
    scancode_table_buffer = table;
    scancode_table_size = size;
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