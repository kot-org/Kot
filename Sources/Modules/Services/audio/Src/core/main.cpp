#include <core/main.h>


int main(int argc, char* argv[]){
    kot_Printlog("[AUDIO] Loading service");
    InitialiseServer();
    kot_Printlog("[AUDIO] Service loaded");

    return KSUCCESS;
}