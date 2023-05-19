#include <math.h>
#include <stdio.h>
#include <string.h>
#include <limits.h>
#include <unistd.h>

int main(int argc, char* argv[]){
  if(argc > 1){
    argv[1][0]++;
    printf("%s\n", argv[1]);
  }else{
    argv[1] = "0";
    printf("%s\n", argv[1]);
  }
  pid_t PPID = fork();
  printf("PPID %x\n", PPID);
  execl(argv[0], argv[0], argv[1], NULL);
  return 0;
}