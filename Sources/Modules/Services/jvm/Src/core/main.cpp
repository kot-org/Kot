#include "main.h"

extern "C" int main(int argc, char* argv[], void* framebuffer, void* _3and4, uintptr_t testClassBuffer) {

    Printlog("[JavaVM] Initialization ...");

    SE8::JVM* jvm = new SE8::JVM();
    jvm->loadClassBytes(testClassBuffer);
    
    Printlog("[JavaVM] Service initialized successfully");
 
    return KSUCCESS;

}