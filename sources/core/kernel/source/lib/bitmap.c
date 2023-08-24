#include <lib/bitmap.h>
#include <lib/memory.h>

void bitmap_init(void* address, size_t size, bitmap_t* bitmap, bool default_value) {
    memset(address, default_value ? 0xff : 0, size);
    *bitmap = (bitmap_t)address;
}