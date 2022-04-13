#pragma once
#include <lib/types.h>
#include <arch/x86-64.h>

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
    vmm_Custom0         = 9, 
    vmm_Custom1         = 10,
    vmm_Custom2         = 11,
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

uint64_t vmm_GetVirtualAddress(void* PhysicalAddress);

bool vmm_GetFlags(pagetable_t table, void* Address, vmm_flag flags);
void vmm_SetFlags(pagetable_t table, void* Address, vmm_flag flags, bool enabled);

uint64_t vmm_Map(void* physicalAddress);
void vmm_Map(void* Address, void* physicalAddress);
void vmm_Map(pagetable_t table, void* Address, void* physicalAddress);
void vmm_Map(pagetable_t table, void* Address, void* physicalAddress, bool user);
void vmm_Map(pagetable_t table, void* Address, void* physicalAddress, bool user, bool readWrite);

void vmm_Unmap(void* Address);
void vmm_Unmap(pagetable_t table, void* Address);
void* vmm_GetPhysical(pagetable_t table, void* Address);

void vmm_CopyPageTable(pagetable_t tableSource, pagetable_t tableDestination, uint64_t from, uint64_t to);
void vmm_Fill(pagetable_t table, uint64_t from, uint64_t to);

void vmm_Swap(pagetable_t table);
uint64_t vmm_Init(struct BootInfo* bootInfo);

pagetable_t vmm_SetupProcess();
pagetable_t vmm_SetupThread(pagetable_t parent);

#define PAGE_SIZE 0x1000
#define VMM_STARTRHALF 0x0
#define VMM_LOWERHALF 0x100
#define VMM_HIGHERALF 0x200
#define VMM_MAXLEVEL 0x4
#define VMM_LEVELENTRY 0x200
#define vmm_GetVirtualAddress(PhysicalAddress)((uint64_t)PhysicalAddress + vmm_HHDMAdress)