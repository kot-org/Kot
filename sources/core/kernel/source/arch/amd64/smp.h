#ifndef _AMD64_SMP_H
#define _AMD64_SMP_H 1

#include <stdint.h>
#include <impl/vmm.h>

struct trampoline_data{
    uint8_t status;
    vmm_space_t paging;
    void* stack;
    void* stack_scheduler;
    void* main_entry;
}__attribute__((packed));

extern struct trampoline_data data_trampoline;
void trampoline_entry(void);
void trampoline_main(void);

#endif