/*#include<sys/defs.h>

#define PGSIZE 	4096

#define PTE_P	0x001	// Present
#define PTE_W	0x002	// Writeable
#define PTE_U	0x004	// User

#define MAX_MEM 32510 
struct memory_map {
                uint64_t addr;
                int map_flag;
                
        };

struct Page{
   struct Page *nextPage;	
};

struct Page *pages;

// page map level 4 
struct PML4{
    uint64_t pt_entry[512];
};

// page directory pointer table
struct PDPT{
    uint64_t pt_entry[512];
};

//page directory table
struct PDT{
    uint64_t pt_entry[512];
};

//page table
struct PT{
    uint64_t pt_entry[512];
};

struct Page *freelist; //free list -- list of available physical memory


//physical memory init 
void phy_mem_limit(uint64_t physfree){

}



void boot_init(size_t numPage){

	pages = (struct Page*)boot_alloc(numPage * sizeof(struct Page));
	page_init();
	struct Page pg 	 = page_alloc(0);



}
struct Page *
page_alloc(int alloc_flags)
{

    struct Page *pg = NULL;
    if (!freelist)
        return NULL;

    pg = freelist;

    freelist = freelist->next;

    if (alloc_flags & ALLOC_ZERO)
        memset(page2kva(pg), 0, PGSIZE);
    return pg;

}

int64_t page_alloc(){
   
   for(int i =0 ; i< MAX_MEM; i++){
        if(mmap[i].map_flag==0){
		mmap[i].map_flag =1;
		mmap[i].ref_cnt++;
	        return mmap[i].addr;
	}
	
	}
	return -1;
   } 
void page_table_alloc(){





}*/
