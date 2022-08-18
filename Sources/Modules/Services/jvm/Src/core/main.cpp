#include "main.h"

extern "C" int main(int argc, char* argv[], void* _, uintptr_t testClassBuffer) {

    Printlog("[JavaVM] Initialization ...");

    SE8::JavaVM* jvm = new SE8::JavaVM();
    jvm->getClasses()->loadClassBytes(testClassBuffer);
    jvm->setEntryPoint("Test");
    jvm->run(NULL, 0);

    Printlog("[JavaVM] Application initialized successfully");
 
    return KSUCCESS;

}