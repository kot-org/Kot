#include <sys/defs.h>
#include <sys/gdt.h>
#include <sys/kprintf.h>
#include <sys/tarfs.h>
#include <sys/ahci.h>
#include <sys/ahcinew.h>
#include <sys/idt.h>
#include <sys/irq.h>
#include <sys/timer.h>
#include <sys/keyboard.h>
#include <sys/task.h>
#include <sys/pmap.h>
#include <sys/str.h>
#include <sys/syscall.h>
#include <sys/vfs.h>
#include <sys/vmmu.h>
#define INITIAL_STACK_SIZE 4096
#define PAGE_SIZE 4096

uint8_t initial_stack[INITIAL_STACK_SIZE]__attribute__((aligned(16)));
uint32_t* loader_stack;
extern char kernmem, physbase;
char stack[4096];
static size_t num_pages_base;   // Number of pages in base memory
static size_t num_pages_upper;  
int max_mem;

char stack[INITIAL_STACK_SIZE];

/*void testContextSwitch()

{
    kprintf("Switching to Sbush!\n");  
    initTasking();
    changeTask();
    kprintf("Returned to Kernel\n");
}
*/
void kmemory()
{

struct file *fp  = kmalloc(sizeof(struct file));

kprintf("tarf open %x", fp);

}

static __inline uint64_t syscall_3(uint64_t n, uint64_t a1, uint64_t a2, uint64_t a3) {
        uint64_t a;
    __asm__ volatile("int $0x80" : "=a" (a) : "0" (n), "D" ((uint64_t)a1), "S" ((uint64_t)a2), "b"((uint64_t)a3));
    return a;

}
void start(uint32_t *modulep, void *physbase, void *physfree)
{
  struct smap_t {
    uint64_t base, length;
    uint32_t type;
  }__attribute__((packed)) *smap;
  while(modulep[0] != 0x9001) modulep += modulep[1]+2;
  for(smap = (struct smap_t*)(modulep+2); smap < (struct smap_t*)((char*)modulep+modulep[1]+2*4); ++smap) {
    if (smap->type == 1 /* memory */ && smap->length != 0) {
    if(smap->base ==0) {
                num_pages_base = smap->length/PAGE_SIZE;
                //kprintf("Lower Memory Pages = %d\n",num_pages_base);
            }
            else {
                num_pages_upper =((smap->base+smap->length) - (uint64_t)physfree)/PAGE_SIZE;
                //kprintf("Higher Memory Pages = %d\n",num_pages_upper);
            }
      kprintf("Available Physical Memory [%p-%p]\n", smap->base, smap->base + smap->length);
      max_mem = (smap->base + smap->length)/PAGE_SIZE;
     // kprintf("Max_mem %d", max_mem);	     
         

}
		
}

clrscr();
memory_init(max_mem, (uint64_t)physfree, (uint64_t)physbase);
tarfs_init();


//readfile check- comment next set of lines after checking
//uint64_t faddr = 0xFFFFFFFF8020D560;
//char fbuf[1024];
//readfile(faddr,1024,(uint64_t)fbuf);




//uint64_t file_add = tarfs_open_file("bin/hello/hello.c");
//char * buf=(char *)page_alloc();
//int fsize = tarfs_read_file(file_add,10, buf);
//kprintf("FileContent:%s,%d",buf,fsize);
init_process_map();
idle_process();
//testContextSwitch(); 

struct task_struct *process = create_process("bin/sbush");
//struct task_struct *process = create_process("bin/cat");
init_process((uint64_t *)stack,process);

//init_process_map();

//struct task_struct *init_proc = create_user_process("bin/hello");
//if(init_proc == NULL)
//		while(1);


kprintf("physfree %p\n", (uint64_t)physfree);
kprintf("physbase %p\n", (int64_t)physbase);
kprintf("tarfs in [%p:%p]\n", &_binary_tarfs_start, &_binary_tarfs_end);
//clrscr();
}

void boot(void)
{
  // note: function changes rsp, local stack variables can't be practically used
  register char *temp2;

  for(temp2 = (char*)0xb8001; temp2 < (char*)0xb8000+160*25; temp2 += 2) 
  *temp2 = 7 ;
  __asm__(
    "cli;"
    "movq %%rsp, %0;"
    "movq %1, %%rsp;"
    :"=g"(loader_stack)
    :"r"(&initial_stack[INITIAL_STACK_SIZE])
  );
  init_gdt(); 
  idt_install();
//  schedule();
/*testing*/
	//char* str = "Himalaya";
          //       syscall_3((uint64_t)1,(uint64_t)1 ,(uint64_t) str,(uint64_t)6);	  
//syscall_init();  
  //setting MSR value
 // pci_probe();
  __asm__ volatile( "sti");	
  start(
    (uint32_t*)((char*)(uint64_t)loader_stack[3] + (uint64_t)&kernmem - (uint64_t)&physbase),
    (uint64_t*)&physbase,
    (uint64_t*)(uint64_t)loader_stack[4]
  );

 /* for(
    temp1 = "!!!!! start() returned !!!!!";temp2 = (char*)0xb8000;
    *temp1;
    temp1 += 1; temp2 += 2)
 *temp2 = *temp1;
  */
  while(1);__asm__ volatile ("hlt");
}

