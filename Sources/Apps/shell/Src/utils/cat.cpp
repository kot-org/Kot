#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "system.h"


int main(int argc, char* argv[]) {
    if(argc != 2){
        printf("Usage : cat <file-path>\n");
        return EXIT_FAILURE;
    }

    FILE* File = fopen(argv[1], "r");
    
    if(File == NULL){
        printf("File '%s' not found\n", argv[1]);
        return EXIT_FAILURE;        
    }

    fseek(File, 0, SEEK_END);
    size_t Size = ftell(File);
    void* Buffer = malloc(Size);
    fseek(File, 0, SEEK_SET);
    fread(Buffer, Size, 1, File);
    printf("%s\n", Buffer);
    free(Buffer);
    fclose(File);
    return EXIT_SUCCESS;
}