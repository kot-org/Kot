#include <stdio.h>
#include <assert.h>

int main(int argc, char* argv[]){
    assert(argc > 0);
    printf("Hello world from %s\n", argv[0]);
    return 0;
}