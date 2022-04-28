#include <boot/boot.h>

static uint8_t stack[KERNEL_STACK_SIZE];

static struct stivale2_header_tag_terminal terminal_hdr_tag = {
    .tag = {
        .identifier = STIVALE2_HEADER_TAG_TERMINAL_ID,
        .next = 0
    },
    .flags = 0
};
 
static struct stivale2_header_tag_framebuffer framebuffer_hdr_tag = {
    .tag = {
        .identifier = STIVALE2_HEADER_TAG_FRAMEBUFFER_ID,
        .next = (uint64_t)&terminal_hdr_tag
    },
    .framebuffer_width  = 0,
    .framebuffer_height = 0,
    .framebuffer_bpp    = 0
};
 
__attribute__((section(".stivale2hdr"), used))
static struct stivale2_header stivale_hdr = {
    .entry_point = 0,
    .stack = (uint64_t)stack + KERNEL_STACK_SIZE,
    .flags = (1 << 1) | (1 << 2) | (1 << 3) | (1 << 4),
    .tags = (uint64_t)&framebuffer_hdr_tag
};
static BootInfo Info;

namespace Boot{
    
    BootInfo* Init(stivale2_struct* stivale2_struct){
        Info.KernelAddress = (stivale2_struct_tag_kernel_base_address*)GetTag(stivale2_struct, STIVALE2_STRUCT_TAG_KERNEL_BASE_ADDRESS_ID);
        Info.Framebuffer = (stivale2_struct_tag_framebuffer*)GetTag(stivale2_struct, STIVALE2_STRUCT_TAG_FRAMEBUFFER_ID);
        Info.Modules = (stivale2_struct_tag_modules*)GetTag(stivale2_struct, STIVALE2_STRUCT_TAG_MODULES_ID);
        Info.Memory = (stivale2_struct_tag_memmap*)GetTag(stivale2_struct, STIVALE2_STRUCT_TAG_MEMMAP_ID);
        Info.PMRs = (stivale2_struct_tag_pmrs*)GetTag(stivale2_struct, STIVALE2_STRUCT_TAG_PMRS_ID);
        Info.HHDM = (stivale2_struct_tag_hhdm*)GetTag(stivale2_struct, STIVALE2_STRUCT_TAG_HHDM_ID);
        Info.Firmware = (stivale2_struct_tag_firmware*)GetTag(stivale2_struct, STIVALE2_STRUCT_TAG_FIRMWARE_ID);
        Info.RSDP = (stivale2_struct_tag_rsdp*)GetTag(stivale2_struct, STIVALE2_STRUCT_TAG_RSDP_ID);
        Info.smbios = (stivale2_struct_tag_smbios*)GetTag(stivale2_struct, STIVALE2_STRUCT_TAG_SMBIOS_ID);
        
        /* Get ramfs */
        for(uint64_t i = 0; i < Info.Modules->module_count; i++){
            if(strcmp(Info.Modules->modules[i].string, "ramfs.bin")){
                Info.ramfs.ramfsBase = (void*)Info.Modules->modules[i].begin;
                Info.ramfs.Size = (size_t)(Info.Modules->modules[i].end - Info.Modules->modules[i].begin);
            }
        }
        
        return &Info;
    }    

    void* GetTag(struct stivale2_struct *stivale2_struct, uint64_t id){
        stivale2_tag* tag = (stivale2_tag*)stivale2_struct->tags;
        while (true)
        {
            if (tag == NULL){
                return NULL;
            }                

            if (tag->identifier == id){
                return tag;
            }

            tag = (stivale2_tag*)tag->next;
        }
    }
}
