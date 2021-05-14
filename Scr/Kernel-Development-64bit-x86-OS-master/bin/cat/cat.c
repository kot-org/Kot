#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(int argc,char* argv[], char* envp[])
{
        char filecont[1024];
        char ccwd[128];
	getcwd(ccwd, 128);
	printf("%s",ccwd);
	strcat(ccwd, argv[1]);
	printf("%s",ccwd);

	//int fd = open("hello.txt", O_RDONLY);
        uint64_t fd = open(ccwd,1);
        if(fd == 0) {
                puts("Cannot open file!");
                return 0;
        }
	
        ssize_t size = 1024;
        read(fd,filecont,1024);
        write(1, filecont, size);
        close(fd);
//	while(1);	
        return 0;
}
