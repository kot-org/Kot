#ifndef __TASK_H__
#define __TASK_H__
//#define PAGE_SIZE		0x1000
#define USER_STACK_TOP		0xF0000000
#define HEAP_START		0x08000000
#define MAX_FD			100 
#define TOTAL_PROCESS		50
#define END 			1
#define RUNNING		        2
#define WAITING			3
#define ZOMBIE			4
#define TRAP_SIZE           5
#define TSS_OFFSET          1
#define KERNEL_STACK_SIZE   512
#define NUM_REGISTERS_SAVED 15


#include <sys/defs.h>

//typedef struct vm_area_struct vma_struct;
//typedef struct mm_struct mm_struct;
 
extern void initTasking();
//extern struct task_struct *current;
//extern struct task_struct *next_process;

int pID[50];

int wait_process(uint64_t childpid,uint64_t status,uint64_t options);
uint64_t kill_process(uint64_t pid);
void idle_process();
 
struct reg_struct 
{
    uint64_t rip;
    uint64_t rflags;
    uint64_t rax,rcx,rdx,rbx;
    uint64_t rsi,rdi,rsp;
    uint64_t r8,r9,r10,r11,r12,r13,r14,r15,cr3;
};

struct mm_struct {
       struct vm_area_struct *mmap;
       struct vm_area_struct *current;
	uint64_t start_code, end_code, start_data, end_data;
	uint64_t start_brk, brk, start_stack;
	uint64_t arg_start, arg_end, env_start, env_end;
	uint64_t rss, total_vm, locked_vm;
};

struct file_struct{
    uint64_t   start;       /* start address of region */
    uint64_t   pgoff;       /* offset in file or NULL */
    uint64_t   size;        /* region initialised to here */
    uint64_t   bss_size;
};


struct vm_area_struct {
        struct mm_struct *mm;
        uint64_t start;              // Our start address within vm_mm
        uint64_t end;                // The first byte after our end address within vm_mm
        struct vm_area_struct *next;            // linked list of VM areas per task, sorted by address
        uint64_t flags;              // Flags read, write, execute permissions
        uint64_t type;               // type of segment its refering to
        struct file_struct *file;           // reference to file descriptors for file opened for writing

};

struct task_struct 
{
    struct reg_struct regs;
    struct task_struct *next;
    struct vm_area_struct *vma_struct;
    uint64_t kstack[512]; 
//    uint64_t task_rsp;
//    uint64_t task_rip;
    char task_name[20];
    struct fd* fd[MAX_FD];	
    int pid ;
    uint64_t sleep_time;		
    int ppid;  	
    int wait_on_child_pid;
    uint64_t num_child; 
    uint64_t task_state;
    char cur_dir[30];
    uint64_t init_kern;
    uint64_t *kernel_rsp;
    uint64_t  kernel_stack;
    uint64_t *stack;

};
typedef struct task_struct task_struct_t; 
//Elf64_Ehdr edr;

struct task_struct* returnWaitTask();
void moveToEnd(struct task_struct* task);
void addTask(struct task_struct *newTask);
extern void createTask(struct task_struct*, void(*)(),uint64_t, uint64_t*);
void schedule();
extern void changeTask();
extern uint64_t sys_fork();
//extern int sys_execve(const char *filename, char *const argv[], char *const envp[]);
extern int sys_execve(char *filename, char **argv, char **envp);
//extern int sys_execve((char *)val1,(char **) val2,(char **)val3);
//extern void switch_to(struct task_struct *previous,struct task_struct *current); // switch from old to new
extern void switchTask();
extern void switch_to_ring3();
extern struct task_struct* init_user_process(char *path);
extern void init_process_map();
extern void schedule();
//extern struct vm_area_struct * allocateVMA(Elf64_Ehdr *ehdr, uint64_t start,uint64_t end,uint64_t flags,uint64_t pheader_offset,uint64_t type, uint64_t p_filesz);
struct task_struct* create_user_process(char *filename);
struct task_struct *create_process(char *binary);
void init_process(uint64_t *stack, struct task_struct *newProc);
void SYS_exit(); 
void task_changedir(uint64_t path);
int sys_process_status();
#endif 
