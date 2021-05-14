#include <stdio.h>
#include<sys/defs.h>
#include<sys/syscall.h>
//#include<syscall.h>
#include<string.h>
#include<stdlib.h>


static int bg_process = 0;
int stdin_parser(char*);
char input[200];
char command[5][64];
char cwd[200];
char inp[200];
void exec_command();



int cd2(char * directorypath){
	
	if (directorypath[0] == '/')
        {
                directorypath++;
        } 
	puts(directorypath);         
        if(chdir(directorypath)){
        //printf("-sbunix: cd: %s: No such file or directory\n", directorypath);
        puts("-sbunix: cd: No such file or directory\n");
        }
	return 0;
}

int cd(char *directorypath) {
        char t_cwd[128];
	char tempcwd[128];
	getcwd(t_cwd,128);
	//printf("%s",t_cwd);
        if(strcmp(directorypath,"\0") ==0 || strcmp(directorypath,"..") ==0 )
        {
		changedir("");
                
        }
        else
        {
                strcpy(tempcwd,t_cwd);
                strcat(tempcwd,directorypath);
                int i = chdir(tempcwd);
                if(i != 0 )
                {
			changedir(tempcwd);
                       // strcpy(cwd,t_cwd);
                        //memset(t_cwd,0,20);
                }
        }
        return 0;
}

void run_script(char * buffer){
char *token; char buff[1024];
        int fd;
        fd = open(buffer,O_RDONLY);
        if (fd == -1){
            puts("This shell-script doesnt exist !!");
            return;
        } else {
            read(fd,buff,1024);
        }
    
        token = strtok(buff,"\n");
        if (token[0] == '#' && token[1] == '!' ) {
            token = strtok(NULL,"\n");
            while (token != NULL)
            {
                char *str = token;
                while(*str == ' ' && *str) str++;
                stdin_parser(str);
                token = strtok(NULL,"\n");
            }
        }
        return;
}

void execcommand(char runPath[], char *command_arguments[], int run_in_background) {

       char *name = "swetha";
       int pid = fork();
       if(pid)
        {
        //printf("Parent\n");
                if (run_in_background == 0)
                {
                //printf("Waiting for return from fork\n");
                //sleep(0);
                waitpid(pid,NULL,0);
                //printf("Return from fork\n");
                }
                else {
                        run_in_background = 0;
                }
        }

    else
        {
        if(strcmp("swetha",name)!=0);
	//puts("in exec command file name-");
        //puts(runPath);
       // execve(runPath, command_arguments, NULL);
        //printf("Returning from exec command %s %s \n", runPath, *command_arguments);
        exit(1);
        }

}

void run_content(int run_in_background)
{
   // printf("Number of args %d \n", number_of_args );
   // printf("The pipe position %d \n", pipe_position );
    //puts("in run content");

    //int bgFlag = run_in_background;
   // char *runPath[10];
              //char *command_arguments[5];
    //runPath[0] = "";

    if ((strcmp(command[0], "cd"))==0)
    {
        cd(command[1]);
        return;
    }
    else if ((strcmp(command[0], "ls"))== 0)
    {
//        runPath[0] = "/bin/ls";
//        command[0] = "/bin/ls";
        strcpy(command[0], "/bin/ls");
      //exec_command(runPath[0], command_arguments, bgFlag,pipe_position);
    }

    else if ((strncmp(command[0], "cat",3))==0)
    {
  //      runPath[0] = "/bin/cat";
        strcpy(command[0], "/bin/cat");
    }

    else if ((strncmp(command[0], "sleep",5))==0)
    {
    //   runPath[0] = "/bin/sleep";
       strcpy(command[0], "/bin/sleep");
    }
    /*else if ((strncmp(command[0], "kill -9",7))==0)
    {
      //  runPath[0] = "/bin/kill-9";
        strcpy(command[0], "/bin/kill");
    }
*/

    else if ((strncmp(command[0], "kill",4))==0)
    {
       if((strncmp(command[1], "-9",2))==0)
     {
        strcpy(command[0], "/bin/kill");
     }

    }

    else if ((strncmp(command[0], "echo",4))==0)
    {
        //runPath[0] = "/bin/echo";
        strcpy(command[0], "/bin/echo");
    }
    else if ((strncmp(command[0], "ps",2))==0)
    {
        //runPath[0] = "/bin/ps";
        strcpy(command[0], "/bin/ps");

    }
    else if ((strncmp(command[0],"sh",2))==0)
    {
        run_script(command[1]);
        return;

    }
    else{
        puts("Command unknown... Please try again");
        puts("\n");
        return;
        }

    exec_command();
        //memset(command,0 , sizeof(command));
}



int stdin_parser(char* input){
        int len = strlen(input);
        if(input[len-1] == '&') //background process
                bg_process = 1;

        //puts(input);
        //puts(" getting the input :  ");

        int j=0,k=0;
        for(int i=0;i<len;i++)
        {
        if(input[i]!='\0'&&input[i]!=' '&&input[i]!='\n')
        command[j][k++]=input[i];
        else
        {command[j][k]='\0';j++;k=0;}
        }
	command[j][k]='\0';
	//strcpy(command[j],NULL);
        //for(int i=0;i<=j;i++)
        //puts(command[i]);
	run_content(bg_process);
        memset(command,0 , sizeof(command));
        return 0;

}

/*
static __inline uint64_t syscall_0(uint64_t n) {

	uint64_t a = -1;  
    __asm volatile("int $0x80" : "=a" (a) : "0" (n));  
    return a;
}

    
uint64_t syscall_3(uint64_t n, uint64_t a1, uint64_t a2, uint64_t a3)
{
    
    uint64_t a = 1;
     __asm__ volatile("int $0x80" : "=a" (a) : "0" (n), "D" ((uint64_t)a1), "S" ((uint64_t)a2), "b"((uint64_t)a3));
    return a;
}

*/

void exec_command() {

               /* char* file_name = "bin/cat";
		char* args[3];
		args[0]= "bin/cat";
		args[1] = "etc/hello.txt";
                args[2] = NULL;
                */
	//int x = 5;
	//x++;

        int pid = fork(); // (int)syscall_0(s_fork);//sys_fork_function();

	if (pid != 0) 
          
           {
//		 char* str = "parent";
//                 syscall_3((uint64_t)1,(uint64_t)1 ,(uint64_t) str,(uint64_t)6);
	//	waitpid(pid,NULL,0);
	//	 while(1);
//		x = 50;
           }

        else {	//while(1);
                 execve(command[0], (char* const*)command,NULL);
//while(1);
//		 char* str = "child";
//                 syscall_3((uint64_t)1,(uint64_t)1 ,(uint64_t) str,(uint64_t)6);
//		x = 100;
//                 syscall_0(s_yield); 
                 //char* str = "childs";
	//	 if(strcmp(str,str)==0)
//	{
///while(1);     
//}
                 
                // syscall_3((uint64_t)1,(uint64_t)1 ,(uint64_t) str,(uint64_t)6);       		
		// exit(1);
               //  while(1);
//		char* file_name = "bin/ls";
//char* str = "sbushs";
//char* args[3];
//args[0]= "bin/cat";
//args[1] = "etc/hello.txt";
//args[2] = "Swetha";
//args[2] = NULL;

//execve(file_name, args,NULL);
			 
	//	char* file_name = "bin/echo";
//		 char* args[3];
		 //args[0]= "bin/echo";
	         //args[1]= "Hello";
		 //args[2]= "Swetha";
//		 execve(file_name, args,NULL);
		//exit(1);*/
	        
	}
		 //char* str1 = "parent again";
                 //syscall_3((uint64_t)1,(uint64_t)1 ,(uint64_t) str1,(uint64_t)6);
	//while(1);
}

/*
int main(int argc, char *argv[], char *envp[]) 
{
char* file_name = "bin/ls";
//char* str = "sbushs";
char* args[2];
//args[0]= "bin/ls";
//args[1] = "etc/hello.txt";
//args[2] = "Swetha";
//args[1] = NULL;

// Check for Open
//char filebuf[1024];
//char *tempfile="etc/hello.txt";
//int fd;
//fd = open(tempfile,1);
//uint64_t fd = 0xFFFFFFFF8020D560;
//read(fd,filebuf,1024);
//printf("%d",fd);

//execve(file_name, args,NULL);
//uint64_t * ptr = (uint64_t *)u_rsp; 
//ptr = (uint64_t) userMain;
//syscall_3((uint64_t)1,(uint64_t)1 ,(uint64_t) str,(uint64_t)6);
exec_command();
//char* file_name = "bin/echo";
//char* args[3];
args[0]= "bin/ls";
args[1]= NULL;
//args[2]= "Swetha";
execve(file_name, args,NULL);
//cd(".");
//kprintf("Hello User world!,%p",&userMain);
while(1);
//  puts("sbush> ");
  return 0;

}
*/


int main(int argc, char *argv[], char *envp[]){
              puts("\n");
	      puts("\t");
	      puts("\t"); 
	      puts("Welcome to SBUnix.....!!");
	      puts("\n");
	      puts("\t");
	      puts("Commands supported: ls , cat, cd, echo, ps");
	      puts("\n");
	      puts("==========================================================================");
	      puts("\n"); 			
              //strcpy(cwd,"bin/");
//	      memset(input,0, 200);		
	      
		while(1)
           	{
			for (int i =0; i<5; i++)
        {
                memset(command[i],0,64);
        }
			//memset(command,0 , sizeof(command));
			memset(input,0, 200);
			getcwd(cwd,200);
                   strcpy(inp, cwd);
                   if(strlen(inp)==0)
                        printf("/");
		   printf("SBUnix/%s>",inp);                            
//                while(1);   
		   //get input from terminal  
		   if(read(0,input,200))
		   {
//			while(1);
	//		puts(input);	
	  	   }
			puts("\n");
//			puts(" ..Printing.. ");	
	
			puts("\n");
         		stdin_parser(input);			   
		   //stdin_parser(input);
                   //memset(input,0, 200);
                
    		}	 
              return 0;
}
