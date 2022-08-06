#include "main.h"

extern "C" int main(int argc, char* argv[], void* framebuffer, void* _3and4, uintptr_t testClassBuffer) {

    Printlog("[JavaVM] Initialization ...");

    JVM8* jvm = new JVM8();
    jvm->loadClassFile(testClassBuffer);

    Printlog("[JavaVM] Service initialized successfully");
 
    return KSUCCESS;

}