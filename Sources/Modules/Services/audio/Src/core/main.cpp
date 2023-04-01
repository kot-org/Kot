#include <core/main.h>


extern "C" int main(int argc, char* argv[]){
    Printlog("[AUDIO] Loading service");
    InitialiseServer();
    Printlog("[AUDIO] Service loaded");


    return KSUCCESS;
}