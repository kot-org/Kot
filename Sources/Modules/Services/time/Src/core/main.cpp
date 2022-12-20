#include <core/main.h>


extern "C" int main(int argc, char* argv[]){
    Printlog("[Time] Loading service");
    InitialiseServer();
    Printlog("[Time] Service loaded");

    return KSUCCESS;
}