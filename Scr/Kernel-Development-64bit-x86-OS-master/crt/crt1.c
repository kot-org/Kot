//#include <stdlib.h>
/*
void _start(void) {
  // call main() and exit() here
__asm__ (
        "movq %rsp, %rbp;"
        "movq 0(%rbp), %rdi;"
        "lea 8(%rbp), %rsi;"
        "lea 16(%rbp, %rdi, 8), %rdx;"
        "call main;"

        "movq %rax, %rdi;"
        "movq $60, %rax;"
        "syscall"
    ); 
*/
// main(0,NULL,NULL);
//}

#include <stdlib.h>
long *sp;
static char* argv[20];
void _start(void) {
  // call main() and exit() here
  __asm__ (  "mov %%rsp, %%rcx"
             : "=c"(sp)
             : // no input operands
          );
//main( *((int *)(sp + 1)), (void*)(sp+2),NULL) ;
int argc = *((int *)(sp + 1));
if(argc>0 && argc<5)
{
//uint64_t argv_addr = (uint64_t)(sp+2);
for(int i=0;i<argc;i++)
{
        argv[i]=((char*)(sp+2+8*i));
//      argv_addr+=0x8;
}
main(argc,argv,NULL);
}else{
main(0,NULL,NULL);
}
exit(1);
}
