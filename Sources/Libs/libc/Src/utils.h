#ifndef _UTILS_H
#define _UTILS_H 1

#define Kot_VendorID 0x107111116

#include <kot/types.h>


#if defined(__cplusplus)
extern "C" {
#endif

static inline bool IsBeetween(uint64_t lower, uint64_t value, uint64_t upper){
    return (lower <= value) && (upper >= value);
}

uint32_t crc32(uint32_t crc, const char *buf, size_t len);

#if defined(__cplusplus)
} 
#endif

#endif