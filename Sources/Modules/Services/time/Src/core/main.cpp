#include <core/main.h>


extern "C" int main(int argc, char* argv[]){
    kot_Printlog("[Time] Loading service");
    InitialiseServer();
    kot_Printlog("[Time] Service loaded");

    return KSUCCESS;
}