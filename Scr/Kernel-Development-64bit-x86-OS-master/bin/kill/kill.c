//#include <stdlib.h>

//#include <unistd.h>
#include <string.h>
//#include <syscall.h>
#include <stdio.h>
#include <stdlib.h>


int main(int argc, char *argv[], char *envp[])
{
        int pid;

        if (argc >= 3)
        {
                pid = atoi(argv[2]);
        }
        else
        {
                return 0;
        }

        if (pid > 0)
                kill(pid);

        return 0;

}

