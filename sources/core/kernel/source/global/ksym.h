#ifndef _GLOBAL_KSYM_H
#define _GLOBAL_KSYM_H 1

#include <global/elf.h>
#include <lib/hashmap.h>

typedef struct{
    elf64_addr address;
    elf64_xword size;
    char name[];
}__attribute__((packed)) ksym_t;

void ksym_init(void);

void ksym_add(const char* symname, void* address);

void* ksym_find(const char* symname);

char* ksym_get_name(void* address);

hashmap_t* ksym_get_map(void);

#endif // _GLOBAL_KSYM_H