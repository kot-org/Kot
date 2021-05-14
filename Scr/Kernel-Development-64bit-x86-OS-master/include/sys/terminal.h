#ifndef _TERMIMAL_H
#define _TERMINAL_H
#include<sys/defs.h>

//static int stdin_flag; // 0->in, 1->out

int readInput(char *buf,int count);
void writeInput();
void clrscr();



#endif
