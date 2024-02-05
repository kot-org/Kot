#ifndef _GLOBAL_KSYM_H
#define _GLOBAL_KSYM_H 1

#include <global/elf.h>

typedef struct{
    elf64_addr address;
    elf64_xword size;
    uint64_t name_len;
    char name[];
}__attribute__((packed)) ksym_t;

void ksym_init(void);

void ksym_add(ksym_t* ksym, bool add_to_kernel_shareable_symbols);

void* ksym_get_address_kernel_shareable_symbols(const char* symname);

char* ksym_get_name(void* address);

#endif // _GLOBAL_KSYM_H