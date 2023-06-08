#include "doomkeys.h"
#include "m_argv.h"
#include "doomgeneric.h"

#include <stdio.h>
#include <unistd.h>

#include <stdbool.h>

void DG_Init(){
  
}

void DG_DrawFrame(){

}

void DG_SleepMs(uint32_t ms){
  
}

uint32_t DG_GetTicksMs(){
 
}

int DG_GetKey(int* pressed, unsigned char* doomKey){
  return 0;
}

void DG_SetWindowTitle(const char * title){

}

int main(int argc, char **argv){
    doomgeneric_Create(argc, argv);

    for (int i = 0; ; i++)
    {
        doomgeneric_Tick();
    }
    

    return 0;
}