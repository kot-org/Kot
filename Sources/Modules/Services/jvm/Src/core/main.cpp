#include "main.h"

extern "C" int main() {
    srv_system_callback_t* File = Srv_System_ReadFileInitrd("Test.class", true);
    SE8::JavaVM* jvm = new SE8::JavaVM();
    jvm->getClasses()->loadClassBytes(File->Data);
    jvm->setEntryPoint("Test");
    jvm->run(NULL, 0);
 
    return KSUCCESS;

}