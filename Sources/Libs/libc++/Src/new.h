#pragma once

#include <stdlib.h>

#if defined(__cplusplus)
inline void* operator new(size_t Size){
    return malloc(Size);
}
inline void* operator new[](size_t Size){
    return malloc(Size);
}

inline void operator delete(void* p){
    free(p);
}
inline void operator delete[](void* p){
    free(p);
}
#endif