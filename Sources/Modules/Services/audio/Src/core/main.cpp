#include <core/main.h>


int main(int argc, char* argv[]){
    kot_Printlog("[AUDIO] Loading service");
    InitialiseServer();
    kot_Printlog("[AUDIO] Service loaded");

    kot_Sys_Close(KSUCCESS); /* Don't close the process */
}