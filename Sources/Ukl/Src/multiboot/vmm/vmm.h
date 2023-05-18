#pragma once
#include <x86/asm.h>
#include <lib/types.h>
#include <lib/utils.h>
#include <multiboot/pmm/pmm.h>
#include <multiboot/memory/memory.h>
#include <multiboot/core/multiboot.h>

#define PML4_HIGHER_HALF_ADDRESS 0xffff800000000000
#define PML5_HIGHER_HALF_ADDRESS 0xff00000000000000

#define STACK_SIZE PAGE_SIZE * 0x10

extern pagetable_t vmm_PageTable;
extern uint64_t vmm_HHDMAdress;

enum vmm_flag{
    vmm_Present         = 0,
    vmm_ReadWrite       = 1,
    vmm_User            = 2,
    vmm_WriteThrough    = 3,
    vmm_CacheDisabled   = 4,
    vmm_Accessed        = 5,
    vmm_LargerPages     = 7,
    vmm_Master          = 9, 
    vmm_IsPureMemory    = 10,
    vmm_Slave           = 11,
};

struct vmm_index{
    uint64_t PDP_i;
    uint64_t PD_i;
    uint64_t PT_i;
    uint64_t P_i;
}__attribute__((packed));

struct vmm_page_table{ 
    uint64_t entries[512];
}__attribute__((packed));

bool vmm_GetFlag(uint64_t* entry, uint8_t flag);
void vmm_SetFlag(uint64_t* entry, uint8_t flag, bool enabled);
uint64_t vmm_GetAddress(uint64_t* entry);
void vmm_SetAddress(uint64_t* entry, uint64_t address);
struct vmm_index vmm_Index(uint64_t virtualAddress);
uint64_t vmm_MapAddress(uint64_t PDP_i, uint64_t PD_i, uint64_t PT_i, uint64_t P_i);

uint64_t vmm_GetVirtualAddress(uint64_t PhysicalAddress);

bool vmm_GetFlags(pagetable_t table, uint64_t Address, vmm_flag flags);
void vmm_SetFlags(pagetable_t table, uint64_t Address, vmm_flag flags, bool enabled);

uint64_t vmm_Map(uint64_t physicalAddress);
void vmm_Map(uint64_t Address, uint64_t physicalAddress);
void vmm_Map(pagetable_t table, uint64_t Address, uint64_t physicalAddress);
void vmm_Map(pagetable_t table, uint64_t Address, uint64_t physicalAddress, bool user);
void vmm_Map(pagetable_t table, uint64_t Address, uint64_t physicalAddress, bool user, bool readWrite);
void vmm_Map(pagetable_t table, uint64_t Address, uint64_t physicalAddress, bool user, bool readWrite, bool isPureMemory);

void vmm_Unmap(uint64_t Address);
void vmm_Unmap(pagetable_t table, uint64_t Address);
uint64_t vmm_GetPhysical(pagetable_t table, uint64_t Address);

void vmm_Init(struct multiboot_tag_mmap* Map, uint64_t* Stack);

void Vmm_ExtractsInfo(struct ukl_boot_structure_t* BootData);

#define VMM_STARTRHALF 0x0
#define VMM_LOWERHALF 0x100
#define VMM_HIGHERALF 0x200
#define VMM_MAXLEVEL 0x4
#define VMM_LEVELENTRY 0x200
#define vmm_GetVirtualAddress(PhysicalAddress)((uint64_t)PhysicalAddress)