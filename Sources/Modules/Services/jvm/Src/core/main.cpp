#include "main.h"

extern "C" int main(int argc, char* argv[], void* _, uintptr_t testClassBuffer) {

    Printlog("[JavaVM] Initialization ...");

    SE8::JVM* jvm = new SE8::JVM();
    jvm->getClasses()->loadClassBytes(testClassBuffer);
    jvm->setEntryPoint("Test");

    jvm->run();

    Printlog("[JavaVM] Application initialized successfully");
 
    return KSUCCESS;

}