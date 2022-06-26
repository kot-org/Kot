#include <core/main.h>


int main(int argc, char* argv[]){
    Printlog("[VGA] initialization ...");
    for(int i = 0; i < argc; i++){
        Printlog(argv[i]);
    }
    Printlog("[VGA] Driver intialized successfully");
    return KSUCCESS;
}