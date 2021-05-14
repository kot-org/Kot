#include <sys/syscall.h>
#include <sys/kprintf.h>
#include <sys/vmmu.h>
#include <sys/task.h>
//#include <sys/str.h>
#include <sys/string.h>
#include <sys/pmap.h>
#include <sys/tarfs.h>
#include <sys/terminal.h>


struct task_struct *currentTask;

struct task_struct *next_process;

int sys_fork_funtion(){

       return (int)sys_fork();
}

int sys_exec_function(char * arg1, char** arg2, char** arg3){
	//kprintf("exec");
	//while(1);
	return (int) sys_execve((char *) arg1, (char **) arg2, (char **) arg3);
}


int sys_waitpid(uint64_t child_pid,uint64_t status,uint64_t options)
{
       
  wait_process(child_pid,status,options);    

  return 0;

}

uint64_t sys_getcwd(char *buf, uint64_t size)
{
    if (size < strlen(currentTask->cur_dir)) {
        return -1;
    }
    strcpy(buf,currentTask->cur_dir);
    return (uint64_t)buf;
    
}


char *check_path(char *cwd,char *path) {
    char *newpath;// *valid;
    if(!path || *path == '\0') {
        return NULL;
    }
   
    newpath = kmalloc(4096);
    if(!newpath) {
        return NULL;
    }
    
    if(*path == '/') {
        strncpy(newpath, path, 4095);
       // valid = NULL;
    } else {
        size_t len;
        len = strlen(cwd);
        strcpy(newpath, cwd);
        if(newpath[len - 1] != '/')
        newpath[len++] = '/';
        //valid = rpath + len - 1;
        strncpy(newpath + len, path, 4096 - len - 1);
    }
    newpath[4095] = '\0';
    //err = _resolve_path(newpath, valid);
    return newpath;


}

int sys_chdir(char *path)
{
/*    char * checkPath;
    checkPath= check_path(current->cur_dir, path);
    if(checkPath == NULL)
    return -1;
    
    if(get_per_ind(checkPath) == 0){
         return -1;
    }
    else
    {
    strcpy(current->cur_dir,checkPath);
    }
    return 1;
*/
	int fno = opendir(path);
        if(fno != 0){
        strcpy(currentTask->cur_dir , (char*)path);
        }
       return 0; 
//        return sy_chdir(path, currentTask);		
}


int sys_getpid(){
    unsigned int prc_pid = (unsigned int) currentTask->pid;
    return prc_pid;
}

int sys_getppid(){
    unsigned int prc_pid = (unsigned int) currentTask->ppid;
    return prc_pid;
}
/*
char *print_state(int task_state){

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

int sys_process_status(){

 kprintf("\t-------------Process List--------\n");
 int count =1;
 struct task_struct *head = currentTask;
// struct task_struct *temp = currentTask;
 while(head!=NULL){
   kprintf("\n %d | %d | %d  | %s |%s",
                                count++,temp->pid,temp->ppid,print_state(temp->task_state));//,temp->pname );
   temp = temp->next;
}while(temp!=head);
	return 0;
}

*/
void syscall_handler(registers_t *r){

	uint64_t sys_num  = r->rax;
	//uint64_t reg_val1 = r->rdi;
	uint64_t reg_val1 = r->rbx;
        uint64_t reg_val2 = r->rsi;
	uint64_t reg_val3 = r->rdx;

	//__asm__ __volatile__("movq %%rbx, %0;":"=a"(reg_val1):);
	//__asm__ __volatile__("movq %%rsi, %0;":"=a"(reg_val2):);
	//__asm__ __volatile__("movq %%rdx, %0;":"=a"(reg_val3):);

	//kprintf("\nSYSCALL han %x  %x %x %x ",sys_num,reg_val1, reg_val1, reg_val1 );

	// kprintf("\n %x", r);
	uint64_t call_result =0;
	switch(sys_num){
	
	case s_read:  call_result = sys_read(reg_val1,(void *) reg_val2, reg_val3); 
	break;
	case s_write:  //kprintf("\nWrite system call");
			call_result = sys_write(reg_val1, (void *)reg_val2, reg_val3);
	//		 __asm__ __volatile__("movq %0, %%rax;" ::"a" ((int64_t)call_result):"cc", "memory");
	break;
        case s_open: call_result = sys_open((char *)reg_val1, reg_val2);
          //               __asm__ __volatile__("movq %0, %%rax;" ::"a" ((int64_t)call_result):"cc", "memory");
        break;
        case s_close: call_result = sys_close(reg_val1);
            //             __asm__ __volatile__("movq %0, %%rax;" ::"a" ((int64_t)call_result):"cc", "memory");

	break;
        case s_fork: call_result = sys_fork_funtion();
              //           __asm__ __volatile__("movq %0, %%rax;" ::"a" ((int64_t)call_result):"cc", "memory");
//	                 switchTask(currentTask, next_process);  
        break;
        case s_chdir: call_result = sys_chdir((char*)reg_val1);
                //         __asm__ __volatile__("movq %0, %%rax;" ::"a" ((int64_t)call_result):"cc", "memory");

        break;
        case s_getcwd: call_result = sys_getcwd((char *)reg_val1,(uint64_t)reg_val2);
                  //       __asm__ __volatile__("movq %0, %%rax;" ::"a" ((int64_t)call_result):"cc", "memory");	
        break;
	  case s_waitpid: call_result = sys_waitpid((uint64_t )reg_val1,(uint64_t)reg_val2, (uint64_t)reg_val3);
                    //     __asm__ __volatile__("movq %0, %%rax;" ::"a" ((int64_t)call_result):"cc", "memory");
        break;
          case s_exit: call_result = sys_exit();	
      
	 break;
          case s_execve: call_result = sys_exec_function((char *)reg_val1,(char**)reg_val2, (char **)reg_val3);
                      //   __asm__ __volatile__("movq %0, %%rax;" ::"a" ((int64_t)call_result):"cc", "memory");
	break;
	 case s_listprocess: call_result = sys_process_status((char *)reg_val1,(char**)reg_val2, (char **)reg_val3);
                        // __asm__ __volatile__("movq %0, %%rax;" ::"a" ((int64_t)call_result):"cc", "memory");
	break;  
	 case s_opendir: call_result = (uint64_t)sys_opendir((char *)reg_val1);
        
	break;
	case s_readdir: call_result = (uint64_t)sys_readdir((char *)reg_val1);
        
	break;
	case s_closedir: sys_closedir((uint64_t)reg_val1);
	break;
	
	case s_changedir: sys_changedir((uint64_t)reg_val1);

        case s_yield: call_result = sys_yield();
              //           __asm__ __volatile__("movq %0, %%rax;" ::"a" ((int64_t)call_result):"cc", "memory");
//                       switchTask(currentTask, next_process);  
        break;

        case s_kill: call_result = sys_kill((uint64_t)reg_val1);

	default: kprintf("Syscall error");
		
	}	
	__asm__ __volatile__("movq %0, %%rax;" ::"a" ((int64_t)call_result):"cc", "memory");
//	r->rax = call_result;
//	*((int64_t *)current->init_kern - 9) = call_result;	 	
	//switchTask(current, next_process);

}


int sys_read(uint64_t fd,char *buf,uint64_t len)
{
        //if standard input
        if(fd == 0){
	return readInput((void *)buf,len);
	}
	else{
	 len =readfile(fd,len, (uint64_t)buf);
	}

        return -1;
}


uint64_t sys_open(char *file_name, int flags)
{

	//kprintf("In sys write");	 
        uint64_t fd;
        fd = openfile(file_name);
//	kprintf("sys_open:%x",fd);
        return fd;
	
}

int sys_close(uint64_t fd){
//kprintf("In sys write");
	//closefile(fd);
	currentTask->fd[fd - 1] = NULL;
	return 1;
}

int sys_write(uint64_t fd, void *buf, int len){
		
	//kprintf("\n%s , %d", buf, len);
        kprintf((char *)buf);
        return len;
        
       // return -1;
}


// init msr registers
void syscall_init() 
{
    uint32_t hi, lo;
   // uint32_t msr;
    
   
    __asm__ __volatile__("rdmsr;" "or $0x1, %%rax;" "wrmsr":: "c"(MSR_EFER));
    
    hi = 0x00130008; 
    lo = 0x00000000; 
   
    //
    __asm__ __volatile__("wrmsr" : : "a"(lo), "d"(hi), "c"(MSR_STAR));
    
    //function to enter syscalls	
    hi = (uint32_t)((uint64_t) syscall_handler >> 32); //lo target RIP
    lo = (uint32_t) (uint64_t) syscall_handler; //high target RIP
    __asm__ __volatile__("wrmsr" : : "a"(lo), "d"(hi), "c"(MSR_LSTAR));
   
    //remove Interrupts from the kernel     
    hi = 0x00000000; //reserved
    lo = 0x00000000; //set EFLAGS Mask
    __asm__ __volatile__("wrmsr" : : "a"(lo), "d"(hi), "c"(MSR_CSTAR));

}



/*   File functions  */

uint64_t sys_exit()
{

	SYS_exit();	
	//currentTask->task_state = ZOMBIE;
	//switchTask(currentTask,next_process);	
        return 0;
    
}

//directory functions

void sys_changedir(uint64_t path)
{

	task_changedir((uint64_t)path);

}

int sys_opendir(char *dir_name){

	return opendir(dir_name);

}

int sys_readdir(char* direct){

	 return readdir(direct);

}


void sys_closedir(uint64_t dir){
 
	 closedir(dir);

}

int sys_yield()

{

schedule();	

return 0;

}


int sys_kill(uint64_t pid)
{

kill_process(pid);

return 0;

}

