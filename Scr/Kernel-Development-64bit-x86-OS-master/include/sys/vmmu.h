#ifndef VMMU_H
#define VMMU_H
#include<sys/defs.h>

#include <sys/pmap.h>

void set_CR3(struct PML4 *);

uint64_t get_CR3();

void* get_phy_addr(uint64_t vaddr);

void* kmalloc();

void* user_Address();

#endif
