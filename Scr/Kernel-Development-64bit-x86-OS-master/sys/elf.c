#include <sys/defs.h>
#include <sys/task.h>
#include <sys/elf64.h>
#include <sys/tarfs.h>
#include <sys/string.h>
#include <sys/pmap.h>
#include <sys/vmmu.h>
#include <sys/kprintf.h>
#define SIZE 512
#define PAGE_SIZE 4096
#define STACK_MEM_TOP 0xf0000000
#define NUM_REGISTERS_SAVED 15
#define TRAP_SIZE           5
#define TSS_OFFSET          1
#define STACK_OFFSET        KERNEL_STACK_SIZE - NUM_REGISTERS_SAVED - TRAP_SIZE - TSS_OFFSET

int load_exe(struct task_struct *task, char *path)
{
	uint64_t elfAddr = getElfHeader(path);
	Elf64_Ehdr *ehdr = (Elf64_Ehdr *)elfAddr;
        Elf64_Phdr *phdr = (Elf64_Phdr *)((uint64_t)ehdr + ehdr->e_phoff); //e_phoff --> This member holds the program header table  file offset in bytes.
	task->regs.rip = ehdr->e_entry;
          int count =0;
	struct vm_area_struct *vma = NULL;
	for(; count < ehdr->e_phnum; count++) 
           
             {
		// with type 1 needs to be loaded. 
		if(phdr->p_type == 1) 

                     {
			vma = allocateVMA(ehdr,phdr->p_vaddr,phdr->p_vaddr + phdr->p_memsz,phdr->p_flags,phdr->p_offset,NOTYPE,phdr->p_filesz);//addVMA();
			task->vma_struct=vma;
			// copy the exe contents to the binary's virtual address  
			uint64_t size = (((vma->end/0x1000)*0x1000 + 0x1000)-((vma->start/0x1000)*0x1000));// rounding off the address to closest multiple of 4096
			uint64_t itr = size/0x1000;
			uint64_t start = (phdr->p_vaddr/0x1000)*0x1000;
			while(itr) {
				uint64_t page = (uint64_t)page_alloc();
				map_process(start, page);
				itr--;
				start += 0x1000; 
			}
			
			if((phdr->p_flags == (PERM_R | PERM_X)) || (phdr->p_flags == (PERM_R | PERM_W)))
                        {
				memcpy((void*)vma->start, (void*)((uint64_t)ehdr + phdr->p_offset), phdr->p_filesz);
			}	
		       
                       }
		phdr++;
	
             }
	//allocate the vma for stack and heap.

get_phy_addr(HEAP_START);

struct vm_area_struct * vma_heap = allocateVMA(NULL,HEAP_START,HEAP_START + PAGE_SIZE,PERM_R | PERM_W,(uint64_t)NULL,HEAP,(uint64_t)NULL);

if(vma != NULL)
	vma->next = vma_heap;
//stack

uint64_t *stack = (uint64_t *)get_phy_addr(USER_STACK_TOP-0x1000);

struct vm_area_struct * vma_stack = allocateVMA(NULL,(uint64_t)stack + PAGE_SIZE,(uint64_t)stack,PERM_R | PERM_W,(uint64_t)NULL,STACK,(uint64_t)NULL);
 
vma_heap->next = vma_stack;

task->stack = (uint64_t *)((uint64_t)stack+0x1000-16);

//task->kernel_rsp = (uint64_t *)&task->kstack[511];

//task->stack = (uint64_t *)STACK_MEM_TOP-0x8;

return 0;

}
