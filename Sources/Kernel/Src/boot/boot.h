#pragma once
#include <boot/stivale2.h>

/* ramfs */
struct ramfs_t{
	uintptr_t ramfsBase;
	size_t Size;
}__attribute__((packed));

/* Boot info stuct */

struct BootInfo{
	struct stivale2_struct_tag_kernel_base_address* KernelAddress;
    struct stivale2_struct_tag_framebuffer* Framebuffer;
    struct stivale2_struct_tag_modules* Modules;
    struct stivale2_struct_tag_memmap* Memory;
    struct stivale2_struct_tag_pmrs* PMRs;
    struct stivale2_struct_tag_hhdm* HHDM;
    struct stivale2_struct_tag_firmware* Firmware;
    struct stivale2_struct_tag_rsdp* RSDP;
    struct stivale2_struct_tag_smbios* smbios;
    
    /* ramfs */
    struct ramfs_t ramfs;
}__attribute__((packed));

namespace Boot{
    struct BootInfo* Init(struct stivale2_struct *stivale2_struct); 
    uintptr_t GetTag(struct stivale2_struct *stivale2_struct, uint64_t id);
}