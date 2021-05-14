#include<sys/pmap.h>
extern char kernmem;
struct PML4 *pml4;
uint64_t cr3;
uint64_t top_vAddr = IDEN_V;

void set_CR3(struct PML4 *new_cr3)
{
	uint64_t base_pgdir_addr = (uint64_t)new_cr3;
	__asm volatile("mov %0, %%cr3":: "b"(base_pgdir_addr));
}

uint64_t get_CR3()
{
	uint64_t saved_cr3;
	__asm volatile("mov %%cr3, %0" : "=r" (saved_cr3));

	return saved_cr3;
}

void* get_phy_addr(uint64_t vaddr)
{
	uint64_t page = (uint64_t)page_alloc();
	map_process(vaddr, page);	
	return (void *)vaddr;
}


void* user_Address()
{
	struct PML4 *newPML4 = (struct PML4 *)page_alloc();
	struct PML4 *curPML4 = (struct PML4 *)get_CR3();
//	curPML4 = (struct PML4 *)((uint64_t)curPML4 | IDEN_V);
//	((struct PML4 *)((uint64_t)newPML4 | IDEN_V))->page_entry[511] = curPML4->page_entry[511];
        newPML4->page_entry[511] = curPML4->page_entry[511];
	return (void *)newPML4;
}

uint64_t kmalloc(uint64_t size)
{
	if(size>0){
		int cnt=0;
		int i=0;
		cnt = size/4096;
		if(size%4096>0)
			cnt++;
		uint64_t ret_addr;
		ret_addr=(uint64_t)(page_alloc());
		for(i=1;i<cnt;i++){
			page_alloc();
		}
		return ret_addr;
	}
	return 0;
}
