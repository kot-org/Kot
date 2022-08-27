#include "main.h"

extern "C" int main() {
    srv_system_fileheader_t* FileHeader = (srv_system_fileheader_t*)malloc(sizeof(srv_system_fileheader_t));
    srv_system_callback_t* File = Srv_System_ReadFileInitrd("Test.class", FileHeader, true);
    SE8::JavaVM* jvm = new SE8::JavaVM();
    jvm->getClasses()->loadClassBytes(FileHeader->Data);
    jvm->setEntryPoint("Test");
    jvm->run(NULL, 0);
 
    return KSUCCESS;

}