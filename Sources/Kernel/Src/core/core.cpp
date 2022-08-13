#include <core/core.h>

extern "C" void main(uintptr_t boot){   
    /* load arch */
    ArchInfo_t* ArchInfo = arch_initialize(boot);

    /* load init file */
    ramfs::File* InitFile = ramfs::FindInitFile();
    
    if(InitFile != NULL){
        uintptr_t BufferInitFile = malloc(InitFile->size);
        ramfs::Read(InitFile, BufferInitFile);
        kthread_t* mainthread;
        ELF::loadElf(BufferInitFile, PriviledgeDriver, &mainthread);
        parameters_t* InitParameters = (parameters_t*)malloc(sizeof(parameters_t));
        SendDataToStartService(ArchInfo, mainthread, InitParameters);
        mainthread->Launch(InitParameters);
        free(InitParameters);
    }else{
        Error("Can't load initialization file");
    }

    
    globalTaskManager->EnabledScheduler(CPU::GetAPICID());

    LaunchUserSpace();

    while(true){
        asm("hlt");
    };
}


