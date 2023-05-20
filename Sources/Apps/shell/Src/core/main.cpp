#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>

#define MAX_ARGS    255
#define MAX_CHAR    1024

char Input[MAX_CHAR];
char* Args[MAX_ARGS];

void GetInput() {
    // reset
    fflush(stdout);
    memset(Input, '\0', MAX_CHAR);

    fgets(Input, MAX_CHAR, stdin);

    Args[0] = strtok(Input, " \n\t");

    uint8_t i = 1;
    while((Args[i] = strtok(NULL, " \n\t")) != NULL && i < MAX_ARGS) {
        if(Args[i][0] == '$')
            Args[i] = getenv(Args[i] + 1);
        
        i++;
    }
}

int main(int argc, char* argv[], char* envp[]) {
    setenv("USER", "seb", 1);

    while(true) {
        // prompt
        printf("> ");

        GetInput();

/*         pid_t pid = fork();

        if(pid == -1) {
            printf("CHILD NOT CREATED");
            break;
        } else if(pid == 0) {

            if(execvp("d1:Kot/System/Apps/echo.elf", NULL) == -1) {
                printf("Command not found");
                //kill(pid, SIGINT);
            }
        } */

        execvpe("d1:Kot/System/Apps/ls.elf", Args, envp);

        memset(Args, '\0', MAX_ARGS);
    }
    
    return 0;
}