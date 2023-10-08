#include <stdio.h>
#include <fcntl.h>
#include <stdint.h>
#include <assert.h>
#include <unistd.h>
#include <stdlib.h>
#include <linux/fb.h>
#include <sys/ioctl.h>

int main(int argc, char* argv[]){
    while(1){
        char op;
        int first, second;
        printf("Enter 'a (+, -, *, /) b': ");
        scanf("%d %c %d", &first, &op, &second);

        switch(op){
            case '+':
                printf("%d + %d = %d\n", first, second, first + second);
                break;
            case '-':
                printf("%d - %d = %d\n", first, second, first - second);
                break;
            case '*':
                printf("%d * %d = %d\n", first, second, first * second);
                break;
            case '/':
                printf("%d / %d = %d\n", first, second, first / second);
                break;
            default:
                printf("Error! operator is not correct : '%c'\n", op);
        }
    }

    return 0;
}