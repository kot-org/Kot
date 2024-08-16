#include <stdio.h>
#include <fcntl.h>
#include <stdint.h>
#include <assert.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <linux/fb.h>
#include <sys/ioctl.h>
#include <sys/types.h>


int main(int argc, char* argv[]){
    setenv("HOME", "/usr", 1);
    setenv("PWD", getenv("HOME"), 1);
    setenv("PATH", "/usr/bin", 1);
    setenv("PATHSTORE", "/usr/store", 1);

    setenv("USER", "root", 1);
    setenv("HOSTNAME", "kot", 1);

    setenv("TERM", "gnome-256color", 1);

    printf("\e[0;33m--- Welcome to Kot ---\e[0m\n");
    printf("If you encounter any bugs during your session, please report them on the github repository:\n\e[0;36mhttps://github.com/kot-org/kot\e[0m\n\n");

    // go back to home 🏠
    chdir(getenv("HOME"));

    setenv("PS1", "\\[\e[0;92m$USER@$HOSTNAME\e[0;37m: \e[0;94m\\w\e[0;37m\\$\e[0m\\] ", 1); // '#' is used to indicate root user session

    pid_t p = fork(); 
    if(p < 0){ 
        perror("init: fork fail"); 
        return EXIT_FAILURE;
    }else if (p == 0){
        char* exe_argv[2] = {"/usr/bin/welcome", NULL};
        execvp("/usr/bin/welcome", exe_argv);
    }

    int status = 0;
    wait(&status);

    char* exe_argv[2] = {"/usr/bin/bash", NULL};
    execvp("/usr/bin/bash", exe_argv);

    perror("init: /usr/bin/bash");
    return EXIT_FAILURE;
}