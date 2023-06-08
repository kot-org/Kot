#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "system.h"

// TODO: usage

int main(int argc, char* argv[]) {
    argv++;
    argc--;
    while(argc > 0) {      
        fputs(argv[0], stdout);
    
        argv++;
        argc--;
        if(argc > 0)
            putchar(' ');
    }
    
    return EXIT_SUCCESS;
}