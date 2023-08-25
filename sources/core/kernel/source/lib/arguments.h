#ifndef LIB_ARGUMENTS_H
#define LIB_ARGUMENTS_H 1


#include <arch/include.h>
/* 
The file in ARCH_INCLUDE(impl/arguments.h) is expected to have :
    - The declaration of macro : MAX_ARGUMENTS_COUNT
*/
#include ARCH_INCLUDE(impl/arguments.h)

typedef struct{
    uintptr_t arg[MAX_ARGUMENTS_COUNT];
}arguments_t;

#endif