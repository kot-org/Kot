#ifndef _MEMORY_H
#define _MEMORY_H 1

#include <kot/types.h>

#if defined(__cplusplus)
extern "C" {
#endif

void memset(void* start, uint8_t value, uint64_t num);
int memcmp(const void *aptr, const void *bptr, size_t n);
void memcpy(void* destination, void* source, uint64_t num);

enum memory_share_flag{
    memory_share_flag_ReadOnly          = 0,
    /* NLA : no live actualization means that once the memory is get by the client the won't be update 
    if the server memory is update also the address give by the server is suppose tu be already alocated
    so it's also read only */
    memory_share_flag_NLA               = 1,
};

bool memory_share_flag_GetFlag(uint64_t* entry, enum memory_share_flag flag);
void memory_share_flag_SetFlag(uint64_t* entry, enum memory_share_flag flag, bool value);

#if defined(__cplusplus)
}
#endif

#endif