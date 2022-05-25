#include <core/core.h>

extern "C" void main(void* boot)
{   
    /* load arch */
    KernelInfo* kernelInfo = arch_initialize(boot);

    /* load init file */
    ramfs::Parse(kernelInfo->ramfs.ramfsBase, kernelInfo->ramfs.Size);
    ramfs::File* InitFile = ramfs::FindInitFile();
    
    if(InitFile != NULL){
        void* BufferInitFile = malloc(InitFile->size);
        ramfs::Read(InitFile, BufferInitFile);
        thread_t* mainThread;
        ELF::loadElf(BufferInitFile, Priviledge_Service, &mainThread);
        Parameters* InitParameters = (Parameters*)malloc(sizeof(Parameters));
        mainThread->ShareDataUsingStackSpace(kernelInfo, sizeof(KernelInfo), &InitParameters->Parameter0);
        InitParameters->Parameter1 = (uint64_t)0x0;
        mainThread->Launch(InitParameters);
        free(InitParameters);
    }else{
        globalLogs->Error("Can't load initialization file");
    }
    
    globalTaskManager->EnabledScheduler(CPU::GetAPICID());

    LaunchUserSpace();

    while(true){
        asm("hlt");
    };
}


