#include "main.h"

extern "C" int main(int argc, char* argv[], void* framebuffer, uintptr_t testClassBuffer) {

    Printlog("[JavaVM] Initialization ...");

    SE8::JVM* jvm = new SE8::JVM();
    jvm->getClasses()->loadClassBytes(testClassBuffer);

    Printlog("[JavaVM] Service initialized successfully");
 
    return KSUCCESS;

}