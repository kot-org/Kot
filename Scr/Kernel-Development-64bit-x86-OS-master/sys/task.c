#include <sys/task.h>
#include <sys/elf64.h>
#include<sys/pmap.h>
#include <sys/vmmu.h>
#include <sys/gdt.h>
#include<sys/string.h>
#include<sys/kprintf.h>
#define NUM_REGISTERS_SAVED 15
#define TRAP_SIZE           5
#define TSS_OFFSET          1
#define STACK_OFFSET        KERNEL_STACK_SIZE - NUM_REGISTERS_SAVED - TRAP_SIZE - TSS_OFFSET
extern void handler_syscall();

struct task_struct *currentTask;

struct task_struct *next_process;
struct task_struct * from ,*to;
struct task_struct *runningTask; //global

int processid[50];

char *print_state(int task_state)

{

        switch (task_state)  {

        case END:
                return "END          ";
        case RUNNING:
                return "RUNNING        ";
        case ZOMBIE:
                return "ZOMBIE         ";
        case WAITING:
                return "WAITING        ";
        default:
                return "UNKNOWN        ";
        }
        return NULL;

}

void task_changedir(uint64_t path)

{

	strcpy(currentTask->cur_dir,(char *)path);

}

int sys_process_status()

{

 kprintf("\t-------------Process List--------\n");
 int count =1;
 struct task_struct *head = currentTask;
 while(head!=NULL){
   kprintf("\n%d |  %d | %d | %s  | %s",count++,head->pid,head->ppid,head->task_name,print_state(head->task_state));//,temp->pname );
   head = head->next;
}
        return 0;
}

static void otherMain() 
{

while(1)

{

	kprintf("Idle Process!\n");
	schedule();

}

}



uint64_t kill_process(uint64_t pid)
{
        //implement kill here
        struct task_struct *kill_Task;
        kill_Task = currentTask;
        while(kill_Task != NULL)
        {
                if(kill_Task->pid==pid)
                        break;
                kill_Task=kill_Task->next;
        }
        if(kill_Task!=NULL)
         {
           kill_Task->task_state = END;
           kprintf("Killed: %d\n",kill_Task->pid);
           sys_process_status();
           schedule();
        }
        return 0;
}

struct task_struct* getTask(uint64_t pid)
{
        struct task_struct *pidTask;
       
        pidTask = currentTask;
       
        while(pidTask != NULL)
         {
       
           if(pidTask->pid==pid)
           
           return pidTask;
           
           pidTask=pidTask->next;       
        
        }
      
return NULL;

}

int wait_process(uint64_t childpid,uint64_t status,uint64_t options)
{
 
     struct task_struct *waitTask = currentTask;
     
    while(waitTask != NULL)
        {
                if(strlen(waitTask->task_name)!=0 && waitTask->ppid == (int)childpid )
                {
                        if(waitTask->task_state==RUNNING)
                        {
                                struct task_struct * parentTask = getTask(childpid);
                                parentTask->task_state = WAITING;
                                schedule();
                        }
                }
                waitTask = waitTask->next;
        }
   
     return 0;
}

void init_process_map()
{
        int id = 1;
        for(; id < 50; id++)
                processid[id] = 0;
}

int procid()
{
         int i=1;
          for(;i<50;i++) {
                if(processid[i] == 0) {
                        processid[i] = 1;
                        return i;
                }
        }
        return -1;
}

void schedule()

{

    struct task_struct * nextTask = returnWaitTask();

    if(nextTask != NULL)
    { 
	from=runningTask;
	to=nextTask;
    	switchTask();
    }
}

struct task_struct *create_process(char *binary)

{	        
        struct task_struct *process = (struct task_struct *)kmalloc(sizeof(struct task_struct));

        process->regs.cr3 = (uint64_t)set_user_AddrSpace();

        struct PML4 *curr_CR3 = (struct PML4 *)get_CR3();

        set_CR3((struct PML4 *)process->regs.cr3);

        load_exe(process,binary);

        process->kstack[511-5] = process->regs.rip;     //RIP
        
        process->kstack[511-4] = 0x2B ;                           //CS
        
        process->kstack[511-3] = 0x246;                           //EFlags
        
        process->kstack[511-2] = (uint64_t)process->stack;     //RSP
        
        process->kstack[511-1] = 0x23 ;                           //SS
        
        process->kernel_rsp = (uint64_t *)&process->kstack[511-5-15];

        set_CR3((struct PML4 *)curr_CR3);
    
        process->next = NULL;
    
        process->pid = procid();   
        strcpy(process->task_name,binary);
        strcpy(process->cur_dir,"bin/");

       // addTask(process);
	currentTask=process;
        return process;
}


void* copy_on_write(struct task_struct *parent)

{
        struct task_struct *child = (struct task_struct *)kmalloc(sizeof(struct task_struct));

//        child->task_state = READY;
        child->pid = procid();
        child->ppid = parent->pid ;
        child->wait_on_child_pid = -1;
        parent->num_child += 1;

        strcpy(child->task_name,parent->task_name);
    
        strcpy(child->cur_dir,parent->cur_dir);    
    
        child->regs.cr3 = (uint64_t)page_alloc();

        setup_child_pagetable(child->regs.cr3);

        
        set_CR3((struct PML4 *)child->regs.cr3);

        
        struct vm_area_struct *parent_vma = parent->vma_struct;
        struct vm_area_struct *child_vma = NULL;
        int first = 0;

        while(parent_vma) {

                if(child_vma == NULL)
                        first = 1;

                child_vma = (struct vm_area_struct *)kmalloc(sizeof(struct vm_area_struct));

                memcpy(child_vma, parent_vma, sizeof(struct vm_area_struct));

                if(parent_vma->file!=NULL){
                        child_vma->file = kmalloc(sizeof(struct file_struct));
                        memcpy(child_vma->file,parent_vma->file,sizeof(struct file_struct));
                }

                if(first) {
                        child->vma_struct = child_vma;
                        first = 0;
                }

                if(child_vma->next) 
                        child_vma = child_vma->next;
                parent_vma = parent_vma->next;
        }

        
        if(!child_vma) {
                child->vma_struct = NULL;
                return (void *)child;
        }

        child_vma->next = NULL;

        return (void *)child;
                                   
}



void idle_process()
{
        
        struct task_struct *idle_task = (struct task_struct *) kmalloc(sizeof(struct task_struct));
        strcpy(idle_task->task_name,"Idle Process");
        idle_task->pid = procid();
        idle_task->ppid = 0;
        idle_task->next = NULL;
        idle_task->regs.cr3 = (uint64_t)get_CR3();
        idle_task->kernel_rsp = (uint64_t *)idle_task->kstack[511];
        idle_task->kstack[511-6]=(uint64_t) otherMain;
        idle_task->task_state = WAITING;
        currentTask = idle_task;

}

void init_process(uint64_t *stack, struct task_struct *newProc)
//This is my switch to ring3 function

{	
        runningTask = newProc;
  
        runningTask->task_state = RUNNING;
   
	struct task_struct *process = newProc ;
     
        __asm __volatile("movq %0, %%cr3":: "a"(process->regs.cr3));

        __asm__ __volatile__ ("movq %0, %%rsp;"::"r"(process->kernel_rsp));      	

         set_tss_rsp((void *)&process->kstack[511]);
       
        __asm__ __volatile__("popq %r15");
	__asm__ __volatile__("popq %r14");
	__asm__ __volatile__("popq %r13");
	__asm__ __volatile__("popq %r12");
	__asm__ __volatile__("popq %r11");
	__asm__ __volatile__("popq %r10");
	__asm__ __volatile__("popq %r9");
	__asm__ __volatile__("popq %r8");
        __asm__ __volatile__("popq %rbp");
	__asm__ __volatile__("popq %rdi");
	__asm__ __volatile__("popq %rsi");
	__asm__ __volatile__("popq %rdx");
	__asm__ __volatile__("popq %rcx");
	__asm__ __volatile__("popq %rbx");
	__asm__ __volatile__("popq %rax");
	__asm__ __volatile__("iretq");

}


void switchTask()
{
    __asm__ __volatile__("addq $8, %rsp");
        runningTask = to;
       
        runningTask->task_state = RUNNING;
        
        if(from->task_state != END)
        {
           from->task_state = WAITING;
        }


    set_CR3((struct PML4 *)to->regs.cr3);

    __asm__ __volatile__("pushq %rax");
    __asm__ __volatile__("pushq %rbx");
    __asm__ __volatile__("pushq %rcx");
    __asm__ __volatile__("pushq %rdx");
    __asm__ __volatile__("pushq %rsi");
    __asm__ __volatile__("pushq %rdi");
    __asm__ __volatile__("movq %%rsp, %0":"=g"(from->kernel_rsp)::"memory");
    __asm__ __volatile__("movq %0, %%rsp"::"m"(to->kernel_rsp));
    __asm__ __volatile__("popq %rdi");
    __asm__ __volatile__("popq %rsi");
    __asm__ __volatile__("popq %rdx");
    __asm__ __volatile__("popq %rcx");
    __asm__ __volatile__("popq %rbx");
    __asm__ __volatile__("popq %rax");
    __asm__ __volatile__("retq");
 
}


uint64_t sys_fork() 
{

		struct task_struct *parent_process = runningTask;

		struct task_struct *child_process = (struct task_struct *)copy_on_write((struct task_struct*)parent_process);

//		child_process->stack=(uint64_t*)USER_STACK_TOP-0x1000;            
                
                child_process->kernel_rsp = (uint64_t *)(&child_process->kstack[511-14-6]);

		child_process->kstack[511-14] = (uint64_t)handler_syscall+0x13;
                
                child_process->kstack[511-13]=parent_process->kstack[511-15];

                child_process->kstack[511-12]=parent_process->kstack[511-14];

                child_process->kstack[511-11]=parent_process->kstack[511-13];

                child_process->kstack[511-10]=parent_process->kstack[511-12];

                child_process->kstack[511-9]=parent_process->kstack[511-11];

                child_process->kstack[511-8]=parent_process->kstack[511-10];

                child_process->kstack[511-7]=parent_process->kstack[511-9];

                child_process->kstack[511-6] = 0; 
                
                child_process->kstack[511-5]=parent_process->kstack[511-7];

                child_process->kstack[511-4] = parent_process->kstack[511-6];
		
                child_process->kstack[511-3] = parent_process->kstack[511-5];                           
		
                child_process->kstack[511-2] = parent_process->kstack[511-4];                           
		
                child_process->kstack[511-1] =  parent_process->kstack[508];                                                   
		
                child_process->kstack[511] = parent_process->kstack[511-2];                           

                set_CR3((struct PML4 *)parent_process->regs.cr3);
                
                strcpy(child_process->cur_dir,parent_process->cur_dir);

                set_tss_rsp((void *)&child_process->kernel_rsp);

                next_process = child_process;
		child_process->next=NULL;
		addTask(child_process);
		from = runningTask;to=child_process;
		switchTask();
		return child_process->pid;

}

void addTask(struct task_struct *newTask)

{

    if(currentTask->next == NULL)

    {
       
       currentTask->next = newTask;
    
    }

    else
    {
        
        struct task_struct *current_pointer = currentTask;
        
         while (1) 
          {

            if(current_pointer->next == NULL)
            
             {
                current_pointer->next = newTask;
                
                break;
             
             }
            
             current_pointer = current_pointer->next;
         }
    }
}


void moveToEnd(struct task_struct* task) 
{
    struct task_struct *task_list = currentTask;

    while (task_list->next != NULL) 

    {
        if (task_list->next == task)

        task_list->next = task_list->next->next;

        task_list = task_list->next;
    }

    task_list->next = task;
    
    task->next = NULL;

}

struct task_struct* returnWaitTask()
{
        struct task_struct *nextTask = currentTask;
        
        while(nextTask != NULL)
        
         {
               
                if(nextTask->task_state == WAITING)
                {
                        moveToEnd(nextTask);

                        return nextTask;
                }
               
                nextTask = nextTask->next;

        }
       
         return NULL;
}

int sys_execve(char *filename, char **argv, char **envp)

{     
       //kprintf("Inside execve %s",filename);
       uint64_t argc=0;

//       char arguments[6][64];
	
       uint64_t index = 0;     
	if(argv != NULL) {
                while((char *)(argv+index) != NULL && strcmp((char *)(argv+index),"")!=0){
               //         strcpy(args[argc],argv[k]);
                        argc++;
                        index+=8;
                }
        }/*
       if(argv != NULL) 

        {
		while(argv[index] != NULL)
                
                {
              		strcpy(arguments[argc],argv[index]);
               		
                        argc++;
			
                        index++;
        	}
	}*/
          
        runningTask->task_state = END;
    
        struct task_struct *binary = (struct task_struct *)kmalloc(sizeof(struct task_struct));	     

        binary->pid = currentTask->pid;

	binary->ppid = currentTask->ppid;      
 
        strcpy(binary->cur_dir,"bin/");

	strcpy(binary->task_name,filename);

        binary->regs.cr3 = (uint64_t)set_user_AddrSpace();

        struct PML4 *curr_CR3 = (struct PML4 *)get_CR3();

        set_CR3((struct PML4 *)binary->regs.cr3);

        load_exe(binary,filename);

        binary->kstack[511-5] = binary->regs.rip;     //RIP

        binary->kstack[511-4] = 0x2B ;                           //CS

        binary->kstack[511-3] = 0x246;                           //EFlags

        binary->kstack[511-1] = 0x23 ;                           //SS

        binary->kernel_rsp = (uint64_t *)&binary->kstack[511-5-15];

        uint64_t *stack = (uint64_t *)get_phy_addr(USER_STACK_TOP-0x1000);

        uint64_t *ptr = (uint64_t *)((uint64_t)stack + 4096-16-64*argc); 
       
        if(argc>0)  
        
        {
       
        *ptr= argc;
        
        memcpy((ptr+1),(void *)argv,argc*64);
        
        }
       
        binary->stack = (uint64_t *)ptr;

        binary->kstack[511-2] = (uint64_t)binary->stack;        

        set_CR3((struct PML4 *)curr_CR3);
       
        //current = binary;
 	
        //init_process((uint64_t)0, binary);    
	binary->next=NULL;

	addTask(binary);//from=runningTask;to=binary;
  
      //  switchTask();
      
        init_process((uint64_t)0, binary);  

        return -1;

}
void SYS_exit()
{
	runningTask->task_state=END;
	schedule();
}
