#include <stdio.h>
#include <stdlib.h>
#include<sys/dirent.h>
#include <string.h>

int main(int argc, char* argv[], char* envp[]){
    
    // writing code for ls-- list all files/ folders in the current directory
/*    puts("In main of ls..!");
    char cwd[1024];
    struct dir *mydir;
    struct dirent *myfile;
    
    getcwd(cwd, sizeof(cwd));
    mydir = opendir(cwd);
    // while(1);
    while((myfile = readdir(mydir)) != NULL)
    {
        puts(myfile->d_name);
    }
    puts("\n");
    while(1);  
    closedir(mydir);
    //while(1);    
    return 1;
*/


     char c_cwd[100];
     //char inp[100];	
     getcwd(c_cwd,100);
     //strcpy(inp, c_cwd);	

    
    if (argc >= 2){
          strcat(c_cwd,argv[1]);
    } else
          getcwd(c_cwd, 100);
    
    
    //getcwd(cwd, sizeof(cwd));
    readdir(c_cwd);
//while(1);
   return 1;	
  
}
