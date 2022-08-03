#include "main.h"

extern "C" int main(int argc, char* argv[]) {

    Printlog("[JVM] Initialization ...");

    JVM8* jvm8 = new JVM8(512*1000);
    jvm8->initialize();

    Printlog("[JVM] Service initialized successfully");
 
    return KSUCCESS;

}