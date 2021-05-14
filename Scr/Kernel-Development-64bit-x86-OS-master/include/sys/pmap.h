#ifndef PMAP_H
#define PMAP_H
#include<sys/defs.h>
#include <sys/elf64.h>
#define PTE_P	0x001	//Present
#define PTE_W	0x002	//Writeable
#define PTE_U	0x004	//User
#define PTE_COW	0x100
#define IDEN_V  0xFFFFFFFF00000000UL	

struct Page{
     int ref_cnt; //avail: 0, busy:1
     struct Page *nextPage;
        uint64_t addr;
};
//Page Map Level 4 Index 
struct PML4 {
        uint64_t page_entry[512];
};

//Page Directory Pointer Index
struct PDPT {
        uint64_t page_entry[512];
};

//Page Direcory Index 
struct PDT {
        uint64_t page_entry[512];
};

//Page Table Index
struct PT {
        uint64_t page_entry[512];
};


void memory_init(int max_mem, uint64_t physfree, uint64_t physbase);
void page_init(int num_pages);
uint64_t get_address(uint64_t* entry);
uint64_t page_alloc();
void page_free(struct Page *page);
void page_table_alloc();
void map_process(uint64_t vaddr, uint64_t paddr);
void map_virt_phys(uint64_t vaddr, uint64_t paddr);
void setup_child_pagetable(uint64_t child_PML4);
uint64_t address_physical(uint64_t vaddr);
uint64_t getPML4_index(uint64_t vaddr);
uint64_t getPDPT_index(uint64_t vaddr);
uint64_t getPDT_index(uint64_t vaddr);
uint64_t getPT_index(uint64_t vaddr);
uint64_t set_user_AddrSpace();
void load_cr3();
struct vm_area_struct * allocateVMA(Elf64_Ehdr *ehdr, uint64_t start,uint64_t end,uint64_t flags,uint64_t pheader_offset,uint64_t type, uint64_t p_filesz);
#endif
