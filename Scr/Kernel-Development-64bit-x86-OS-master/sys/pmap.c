#include<sys/pmap.h>
#include<sys/kprintf.h>
#include<sys/vmmu.h>

#define FRAME 0xFFFFFFFFFFFFF000
uint64_t physfree;
//uint64_t physbase;

struct PML4 *pml4;
struct Page* page;
struct Page* freeList; // list of free pages
extern char kernmem, physbase;
uint64_t cr3;
void* meemset(void *str,int val,uint64_t size)
{
    unsigned char* ptr = str;
    while(size--)
        *ptr++ = (unsigned char) val;
    return str;
}

void load_cr3() {

	uint64_t baseaddr = (uint64_t)cr3;
	__asm volatile("movq %0, %%cr3":: "r"(baseaddr):"memory");	
//	kprintf("CR3 loaded");	
//	kprintf("%x", page_alloc()->addr);
}

uint64_t get_address(uint64_t* entry)
{
    return (*entry & FRAME);
}
struct vm_area_struct * allocateVMA(Elf64_Ehdr *ehdr, uint64_t start,uint64_t end,uint64_t flags,uint64_t pheader_offset,uint64_t type, uint64_t p_filesz) // should be called in loop for each phdr
{

struct vm_area_struct *vma = (struct vm_area_struct *)kmalloc((uint64_t)4096);
vma->start = start;
vma->end =  end;
vma->flags = flags;
vma->file = NULL;
vma->next = NULL;
vma->type = type; 

if(flags == (PERM_R | PERM_X)) // text segment is only readable/executable
{

       vma->type = TEXT;

}

else
{
    vma->type = DATA;
}

return vma;
}

void memory_init(int max_mem, uint64_t free, uint64_t base){

	physfree = free;
	//physbase = base;
	page = (struct Page*)physfree;
	page_init(max_mem);	
	page_table_alloc();
	//video memory mapping
	uint64_t viraddr = physfree;//0xFFFFFFFF00000000UL;
        while(viraddr<=0x500000)
        {
                map_virt_phys(viraddr,viraddr);
                viraddr+=0x1000;
        }
	
	map_virt_phys(0xffffffff800b8000UL,0xb8000UL);
	load_cr3();
}
 
void page_init(int num_pages){
	size_t i;
	struct Page* prevPtr = NULL;
	size_t memory = (physfree + (num_pages*sizeof(struct Page *)))/4096   ;
	//kprintf("busy memory %d\t", sizeof(struct Page ));
	for( i=0 ; i < num_pages; i++ ){
		// kprintf("busy memory %d\t", memory);		
		//memory = i * 4096;
		page[i].addr = i*4096;
		if( i  <  memory  ){
			page[i].ref_cnt = 1;
			page[i].nextPage = NULL;
		//	page[i].addr = i*4096;
		}
		else{
			//if(i<600){
			//kprintf("value i: %x\t->%x", &page[i],&page[i].nextPage );
			//}	
			page[i].ref_cnt = 0;
			//page[i].nextPage = freeList;
			//freeList = &page[i];
			
			if(prevPtr != NULL){
				prevPtr->nextPage = &page[i];
				if(i<600)
				{
				//	kprintf("value prev: %p\t->%p,%p", prevPtr,&page[i],&page[i-1]);
				}
			}
			else{
			freeList = &page[i];
			//freeList = &page[i] + sizeof(struct Page*);
			}
			//page[i].nextPage = freeList;
			prevPtr = &page[i];
			//if(i > num_pages-10){
			//kprintf("value i: %x\t",freeList );
//			}
 			//if(i<600){
                       // kprintf("value i: %x\t->%x", &page[i],&page[i].nextPage );
                        //}
		}			
		
	
}
	int j=589;
	while(j<600)
        {
           //  kprintf("value i: %p\t->%p", &page[j],page[j].nextPage );
		j++;
        }	
}


uint64_t page_alloc(){
	
	struct Page *newPage = NULL;
	//kprintf("\n in page alloc, %x", freeList);
	if(!freeList) return -1UL;
	newPage = freeList;
	freeList = freeList->nextPage;
        newPage->ref_cnt =1;
	//meemset(newPage, 0, 4096);
	return newPage->addr;
}

void page_free(struct Page *page){

	page->nextPage = freeList;
	freeList =page;

}


uint64_t getPML4_index(uint64_t vaddr){
	return	((vaddr) >> 39) & 0x1FF ;
}

uint64_t getPDPT_index(uint64_t vaddr){
        return  (((vaddr) >> 30) & 0x1FF) ;
}

uint64_t getPDT_index(uint64_t vaddr){
        return  (((vaddr) >> 21) & 0x1FF) ;
}

uint64_t getPT_index(uint64_t vaddr){
        return  (((vaddr) >> 12) & 0x1FF) ;
}

struct PDPT* pdpt_alloc(struct PML4 *pml4, uint64_t pml4Index){
	struct PDPT *pdpt = (struct PDPT*)page_alloc();
	//kprintf("\n in pdpt alloc, %x", pdpt);
	uint64_t pdpt_entry = (uint64_t)pdpt;
	pdpt_entry |= (PTE_P|PTE_W|PTE_U);
	pml4->page_entry[pml4Index] = pdpt_entry;
	return (void *)pdpt;
}

struct PDT* pdt_alloc(struct PDPT *pdpt, uint64_t pdptIndex){
        struct PDT *pdt = (struct PDT *)page_alloc();
        //kprintf("\n in pdt alloc, %x", pdt);
	uint64_t pdt_entry = (uint64_t)pdt;
        pdt_entry |= (PTE_P|PTE_W|PTE_U);
        pdpt->page_entry[pdptIndex] = pdt_entry;
        return pdt;
}

struct PT* pt_alloc(struct PDT *pdt, uint64_t pdtIndex){
        struct PT *pt = (struct PT *)page_alloc();
	//kprintf("\n in pt alloc, %x", pt);
        uint64_t pt_entry = (uint64_t)pt;
        pt_entry |= (PTE_P|PTE_W|PTE_U);
        pdt->page_entry[pdtIndex] = pt_entry;
        return pt;
}

//initalizing page tables
void page_table_alloc(){
	//struct PML4 *pml4;
	struct PDPT *pdpt;
	struct PDT *pdt;
	struct PT *pt;

	uint64_t vaddr =(uint64_t) &kernmem; 
	uint64_t physb = (uint64_t)&physbase;	
	pml4 = (struct PML4*)page_alloc();
	cr3 = (uint64_t)pml4;

	if(!pml4)
	{
	kprintf("No pml4");
	return;
	}
	else{
 	//kprintf("PML4 allocated");
	}
	uint64_t pml4_index = getPML4_index((uint64_t)vaddr);
        uint64_t pdpt_index = getPDPT_index((uint64_t)vaddr);
        uint64_t pdt_index  = getPDT_index((uint64_t)vaddr); 
	
	pdpt = pdpt_alloc(pml4, pml4_index);
	if(!pdpt){
	//kprintf("\n No pdpt");
        return;
	}	
	pdt = pdt_alloc(pdpt, pdpt_index);
	if(!pdt)
		return;

	pt = pt_alloc(pdt, pdt_index);
	if(!pt)
		return;

	// mapping the kernel from physbase to physfree 
	for(;physb<(physfree);physb += 0x1000, vaddr += 0x1000) {
        	uint64_t index = getPT_index((uint64_t)vaddr);
		uint64_t entry = physb;
		entry |= (PTE_P|PTE_W|PTE_U);
		pt->page_entry[index] = entry;
	}


}

uint64_t get_pml4_entry(struct PML4 **pml4, uint64_t pml4_indx)
{
//	*pml4 = (struct PML4 *)((uint64_t)*pml4 | IDEN_V);
	uint64_t pml4_entry = (*pml4)->page_entry[pml4_indx];
	
	return pml4_entry;
}
uint64_t get_pdpt_entry(struct PDPT** pdpt, uint64_t pdpt_indx)
{
//	*pdpt = (struct PDPT *)((uint64_t)*pdpt | IDEN_V);
	uint64_t pdpt_entry = (*pdpt)->page_entry[pdpt_indx];

	return pdpt_entry;	
}

/* returns the pdt entry */
uint64_t get_pdt_entry(struct PDT **pdt, uint64_t pdt_indx)
{
//	*pdt = (struct PDT *) ((uint64_t) *pdt | IDEN_V);
	uint64_t pdt_entry = (*pdt)->page_entry[pdt_indx];

	return pdt_entry;
}

/* returns the pt entry */
uint64_t get_pt_entry(struct PT **pt, uint64_t pt_indx)
{
//	*pt = (struct PT *)((uint64_t) *pt | IDEN_V);
	uint64_t pt_entry = (*pt)->page_entry[pt_indx];

	return pt_entry;
}


void map_process(uint64_t vaddr, uint64_t paddr)
{
        struct PDPT     *pdpt;
        struct PDT      *pdt;
        struct PT       *pt;

	uint64_t pml4_indx = getPML4_index((uint64_t)vaddr);
        uint64_t pdpt_indx = getPDPT_index((uint64_t)vaddr);
        uint64_t pdt_indx =  getPDT_index((uint64_t)vaddr);
        uint64_t pt_indx =   getPT_index ((uint64_t)vaddr);

	struct PML4 *pml4 = (struct PML4*) get_CR3();

	//pml4 = (struct PML4*) (IDEN_V | (uint64_t) pml4); 
        uint64_t pml4_entry = pml4->page_entry[pml4_indx];

        if(pml4_entry & PTE_P)
                pdpt = (struct PDPT *)get_address(&pml4_entry);
        else
		pdpt = (struct PDPT*)pdpt_alloc(pml4, pml4_indx); 


	uint64_t pdpt_entry = get_pdpt_entry(&pdpt, pdpt_indx);
        if(pdpt_entry & PTE_P)
                pdt = (struct PDT*)get_address(&pdpt_entry);
        else
		pdt = (struct PDT*)pdt_alloc(pdpt, pdpt_indx);


	uint64_t pdt_entry = get_pdt_entry(&pdt, pdt_indx);
        if(pdt_entry & PTE_P)
                pt = (struct PT*)get_address(&pdt_entry);
        else
		pt = (struct PT*)pt_alloc(pdt, pdt_indx);


//	pt = (struct PT*)((uint64_t) pt | IDEN_V);
        uint64_t entry = paddr;
        entry |= (PTE_P|PTE_W|PTE_U);

        pt->page_entry[pt_indx] = entry;

}

 

void map_virt_phys(uint64_t vaddr,uint64_t paddr){
        struct PDPT *pdpt;
        struct PDT *pdt;
        struct PT *pt;

        uint64_t pml4Index = getPML4_index((uint64_t)vaddr);
        uint64_t pdptIndex = getPDPT_index((uint64_t)vaddr);
        uint64_t pdtIndex = getPDT_index((uint64_t)vaddr);
        uint64_t ptIndex = getPT_index((uint64_t)vaddr);

        uint64_t pml4_entry =  pml4->page_entry[pml4Index];
        if(pml4_entry & PTE_P)
                pdpt = (struct PDPT *)get_address(&pml4_entry);
        else
                pdpt = (struct PDPT*)pdpt_alloc(pml4, pml4Index);

        uint64_t pdpt_entry = pdpt->page_entry[pdptIndex];
        if(pdpt_entry & PTE_P)
                pdt = (struct PDT*)get_address(&pdpt_entry);
        else
                pdt = (struct PDT*)pdt_alloc(pdpt, pdptIndex);

        uint64_t pdt_entry = pdt->page_entry[pdtIndex];
        if(pdt_entry & PTE_P)
                pt = (struct PT*)get_address(&pdt_entry);
        else
                pt = (struct PT*)pt_alloc(pdt, pdtIndex);

        uint64_t entry = paddr;
        entry |= (PTE_P|PTE_W|PTE_U);
        pt->page_entry[ptIndex] = entry;

}
uint64_t set_user_AddrSpace()
{
	struct PML4 *newPML4 = (struct PML4 *)page_alloc();
	struct PML4 *curPML4 = (struct PML4 *)get_CR3();

	//curPML4 = (struct PML4 *)((uint64_t)curPML4 | IDEN_V);

	/* map the kernel page table for the process */
for(int i=0;i<=511;i++)	
newPML4->page_entry[i] = curPML4->page_entry[i];

	return (uint64_t)newPML4;
}
void setup_child_pagetable(uint64_t child_PML4)
{
	struct PML4 *c_pml4 = (struct PML4 *)child_PML4;
	struct PML4 *p_pml4 = (struct PML4 *)get_CR3();

	int pml4_indx = 0;
	for(; pml4_indx < 510; pml4_indx++) {

		uint64_t pml4_entry = get_pml4_entry(&p_pml4, pml4_indx);

		if(pml4_entry & PTE_P) {

			struct PDPT *c_pdpt = (struct PDPT *)pdpt_alloc(c_pml4, pml4_indx);

			struct PDPT *p_pdpt = (struct PDPT *) get_address(&pml4_entry);
			int pdpt_indx = 0;
			for(; pdpt_indx < 512; pdpt_indx++) {

				uint64_t pdpt_entry = get_pdpt_entry(&p_pdpt, pdpt_indx);
				if(pdpt_entry & PTE_P) {

					//struct PDPT *tmp_pdpt = c_pdpt; 
					struct PDT *c_pdt = (struct PDT *)pdt_alloc(c_pdpt, pdpt_indx);

					struct PDT *p_pdt = (struct PDT *) get_address(&pdpt_entry);
					int pdt_indx = 0;
					for(; pdt_indx < 512; pdt_indx++) {

						uint64_t pdt_entry = get_pdt_entry(&p_pdt, pdt_indx);
						if(pdt_entry & PTE_P) {

							//struct PDT *tmp_pdt = c_pdt;
							struct PT *c_pt = (struct PT *)pt_alloc(c_pdt, pdt_indx);
	
							struct PT *p_pt = (struct PT *)get_address(&pdt_entry);
							int pt_indx = 0;
							for(; pt_indx < 512; pt_indx++) 
                                                             {
								
                                                                 uint64_t pt_entry = get_pt_entry(&p_pt, pt_indx);
								 
                                                                      if(pt_entry & PTE_P) {
									
                                                                        uint64_t page = (uint64_t)get_address(&pt_entry);                                                                            
								 	 pt_entry = page | PTE_P | PTE_U | PTE_COW;	                                                                   
                                                                         c_pt->page_entry[pt_indx] = pt_entry; 
									 pt_entry = page | (PTE_P | PTE_U | PTE_COW);
									 p_pt->page_entry[pt_indx] = pt_entry;
                                                                       	
                                                                         //memcpy(c_pt->page_entry[pt_indx] ,p_pt->page_entry[pt_indx],sizeof(page));
								}	
							
							}	
						}		
					}		 
				}	
			}	
		}
	}

	c_pml4->page_entry[511] = p_pml4->page_entry[511];
	c_pml4->page_entry[510] = p_pml4->page_entry[510];
}
//physical page management


uint64_t address_physical(uint64_t vaddr)

{
        uint64_t        paddr = 0;
        struct PDPT     *pdpt = NULL;
        struct PDT      *pdt = NULL;
        struct PT       *pt = NULL;

        uint64_t pml4Index = getPML4_index((uint64_t)vaddr);
        uint64_t pdptIndex = getPDPT_index((uint64_t)vaddr);
        uint64_t pdtIndex = getPDT_index((uint64_t)vaddr);
        uint64_t ptIndex = getPT_index((uint64_t)vaddr);

        struct PML4 *pml4 = (struct PML4*) get_CR3();
        uint64_t pml4_entry = pml4->page_entry[pml4Index];

        if(pml4_entry & PTE_P)
                pdpt = (struct PDPT *)get_address(&pml4_entry);

        uint64_t pdpt_entry = get_pdpt_entry(&pdpt, pdptIndex);

        if(pdpt_entry & PTE_P)
                pdt = (struct PDT*)get_address(&pdpt_entry);

        uint64_t pdt_entry = get_pdt_entry(&pdt, pdtIndex);
        if(pdt_entry & PTE_P)
                pt = (struct PT*)get_address(&pdt_entry);
        if(pt != NULL)
                paddr = get_pt_entry(&pt, ptIndex);

        return paddr;

}
	

/*page_alloc()
	page_free()

//page table management

pml4e_walk()
        pdpe_walk()
        pgdir_walk()
        boot_map_region()
        page_lookup()
        page_remove()
        page_insert()



*/
