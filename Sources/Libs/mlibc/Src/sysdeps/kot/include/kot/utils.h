#ifndef KOT_UTILS_H
#define KOT_UTILS_H 1

#define Kot_VendorID 0x107111116

#include <kot/types.h>


#if defined(__cplusplus)
extern "C" {
#endif

static inline bool kot_IsBeetween(uint64_t lower, uint64_t value, uint64_t upper){
    return (lower <= value) && (upper >= value);
}

uint32_t kot_crc32(uint32_t crc, const char *buf, size_t len);

#if defined(__cplusplus)
} 
#endif

#endif