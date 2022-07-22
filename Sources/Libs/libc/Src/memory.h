#ifndef _MEMORY_H
#define _MEMORY_H 1

#include <kot/types.h>
#include <kot/heap.h>

#if defined(__cplusplus)
extern "C" {
#endif

void memset(uintptr_t start, uint8_t value, uint64_t num);
void memset16(uintptr_t start, uint16_t value, uint64_t num);
void memset32(uintptr_t start, uint32_t value, uint64_t num);
void memset64(uintptr_t start, uint64_t value, uint64_t num);

int memcmp(const void *aptr, const void *bptr, size_t n);
void memcpy(uintptr_t destination, uintptr_t source, uint64_t num);

enum memory_share_flag{
    memory_share_flag_ReadOnly          = 0,
    /* NLA : no live actualization means that once the memory is get by the client the won't be update 
    if the server memory is update also the address give by the server is suppose tu be already alocated */
    memory_share_flag_NLA                = 1,
    memory_share_flag_User               = 2,
};

bool memory_share_flag_GetFlag(uint64_t* entry, enum memory_share_flag flag);
void memory_share_flag_SetFlag(uint64_t* entry, enum memory_share_flag flag, bool value);

#if defined(__cplusplus)
}
#endif

#endif