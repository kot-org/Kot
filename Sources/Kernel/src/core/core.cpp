#include <core/core.h>

extern "C" void main(uintptr_t boot){   
    /* load arch */
    ArchInfo_t* ArchInfo = arch_initialize(boot);

    /* load init file */
    ramfs::Parse(ArchInfo->ramfs.ramfsBase, ArchInfo->ramfs.Size);
    ramfs::File* InitFile = ramfs::FindInitFile();
    
    if(InitFile != NULL){
        uintptr_t BufferInitFile = malloc(InitFile->size);
        ramfs::Read(InitFile, BufferInitFile);
        thread_t* mainThread;
        ELF::loadElf(BufferInitFile, PriviledgeDriver, &mainThread);
        parameters_t* InitParameters = (parameters_t*)malloc(sizeof(Parameters));
        mainThread->ShareDataUsingStackSpace(ArchInfo, sizeof(ArchInfo_t), &InitParameters->Parameter0);
        mainThread->Launch(InitParameters);
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


