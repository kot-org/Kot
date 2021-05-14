#include <stdlib.h>
#include <string.h>
#include <stdio.h>
int main(int argc , char* argv[], char* env[]){
  	int i = 0;
	//char* buf;
	if(argc >= 2)
	{
	for(i=1; i< argc; i++)
        {
	puts(argv[i]);		
	puts(" ");
	}
	}//while(1);
}  

