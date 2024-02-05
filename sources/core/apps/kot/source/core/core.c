#include <stdio.h>
#include <stdlib.h>
#include <signal.h>

void sigint_handler(){
    printf("Signal\n");
}

int main(int argc, char* argv[]){
    printf("Hello\n");
    signal(SIGINT, sigint_handler); 
    while(1);  
    return EXIT_SUCCESS;
}