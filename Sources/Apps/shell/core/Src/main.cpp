#include <stdio.h>

extern "C" int main() {
    while(true){
        char c = getchar();
        printf("%x\n", c);
    }

    return 0;
}