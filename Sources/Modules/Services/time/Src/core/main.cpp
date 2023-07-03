#include <core/main.h>


int main(int argc, char* argv[]){
    kot_Printlog("[Time] Loading service");
    InitialiseServer();
    kot_Printlog("[Time] Service loaded");

    kot_Sys_Close(KSUCCESS); /* Don't close the process */
}