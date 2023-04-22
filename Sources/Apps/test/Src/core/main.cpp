#include <stdio.h>

extern "C" void* __dso_handle = 0;

extern "C" int main(){
    printf("Hello from mlibc, with %s %x", "stdio.h", __dso_handle);
    return 0;
}