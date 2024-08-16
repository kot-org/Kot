#include <stdio.h>
#include <fcntl.h>
#include <stdint.h>
#include <assert.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdbool.h>
#include <sys/wait.h>
#include <linux/fb.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <sys/types.h>

static bool is_file_exists(char* path){
    struct stat sb;   
    return (stat(path, &sb) == 0);
}

int main(int argc, char* argv[]){
    bool is_welcoming = !is_file_exists("/usr/share/info/done_welcomig");

    setenv("HOME", "/usr", 1);
    setenv("PWD", getenv("HOME"), 1);
    setenv("PATH", "/usr/bin", 1);
    setenv("PATHSTORE", "/usr/store", 1);

    setenv("USER", "root", 1);
    setenv("HOSTNAME", "kot", 1);

    setenv("TERM", "gnome-256color", 1);


    // go back to home 🏠
    chdir(getenv("HOME"));

    setenv("PS1", "\\[\e[0;92m$USER@$HOSTNAME\e[0;37m: \e[0;94m\\w\e[0;37m\\$\e[0m\\] ", 1); // '#' is used to indicate root user session

    if(is_welcoming){

        pid_t p = fork(); 
        if(p < 0){ 
            perror("init: fork fail"); 
            return EXIT_FAILURE;
        }else if(p == 0){
            FILE* done_welcoming_file = fopen("/usr/share/info/done_welcomig", "w");
            fclose(done_welcoming_file);

            char* exe_argv[2] = {"/usr/bin/welcome", NULL};
            execvp("/usr/bin/welcome", exe_argv);

            perror("init: /usr/bin/welcome");
            return EXIT_FAILURE;
        }

        int status = 0;
        wait(&status);
    }

    printf("\e[0;33m--- Welcome to Kot ---\e[0m\n");
    printf("If you encounter any bugs during your session, please report them on the github repository:\n\e[0;36mhttps://github.com/kot-org/kot\e[0m\n\n");

    char* exe_argv[2] = {"/usr/bin/bash", NULL};
    execvp("/usr/bin/bash", exe_argv);

    perror("init: /usr/bin/bash");
    return EXIT_FAILURE;
}