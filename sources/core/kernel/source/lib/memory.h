#ifndef _MEMORY_H
#define _MEMORY_H 1

#include <stdint.h>
#include <stddef.h>

typedef struct{
    void* address;
    size_t size;
}memory_range_t;

typedef uint8_t memory_flags_t;

#define MEMORY_FLAG_READABLE            (1 << 0)
#define MEMORY_FLAG_WRITABLE            (1 << 1)
#define MEMORY_FLAG_EXECUTABLE          (1 << 2)
#define MEMORY_FLAG_USER                (1 << 3)
#define MEMORY_FLAG_DMA                 (1 << 4)

void *memcpy(void *dest, const void *src, size_t n);
void *memset(void *s, int c, size_t n);
void *memmove(void *dest, const void *src, size_t n);
int memcmp(const void *s1, const void *s2, size_t n);

#endif // _MEMORY_H
