#ifndef _BITMAP_H
#define _BITMAP_H 1

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

typedef uint8_t* bitmap_t;

void bitmap_init(void* address, size_t size, bitmap_t* bitmap, bool default_value);

static inline bool bitmap_get_bit(bitmap_t bitmap, size_t index) {
    return bitmap[index / 8] & (1 << (index % 8));
}

static inline void bitmap_set_bit(bitmap_t bitmap, size_t index, bool value) {
    if(value) {
        bitmap[index / 8] |= (1 << (index % 8));
    }else{
        bitmap[index / 8] &= ~(1 << (index % 8));
    }
}

static inline void bitmap_set_bits(bitmap_t bitmap, size_t index, size_t count, bool value) {
    for (size_t i = 0; i < count; i++) {
        bitmap_set_bit(bitmap, index + i, value);
    }
}

static bool bitmap_get_set_bit(bitmap_t bitmap, uint64_t index, bool value) {
    bool current_value = bitmap_get_bit(bitmap, index);
    
    if (current_value != value) {
        bitmap_set_bit(bitmap, index, value);
    }

    return current_value;
}

#endif // _BITMAP_H
