#include <core/main.h>

extern "C" int main(int argc, char* argv[]) {

    Printlog("[JVM] Initialization ...");

    JVM8 jvm8 = JVM8();

    jvm8.initialize();

    Printlog("[JVM] Service initialized successfully");
 
    return KSUCCESS;

}