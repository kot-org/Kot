#include <core/core.h>

extern "C" void main(void* boot)
{   
    /* load arch */
    KernelInfo* kernelInfo = arch_initialize(boot);

    /* load init file */
    RamFS::Parse(kernelInfo->ramfs->RamFsBase, kernelInfo->ramfs->Size);
    RamFS::File* InitFile = RamFS::FindInitFile();
    
    Parameters* InitParameters = (Parameters*)malloc(sizeof(Parameters));
    InitParameters->Parameter0 = (uint64_t)kernelInfo;
    if(InitFile != NULL){
        void* BufferInitFile = malloc(InitFile->size);
        Read(InitFile, BufferInitFile);
        ELF::loadElf(BufferInitFile, 1, InitParameters);
    }else{
        globalLogs->Error("Can't load initialization file");
    }
    
    free(InitParameters);

    
    globalTaskManager->EnabledScheduler(CPU::GetCoreID());

    LaunchUserSpace();

    while(true){
        asm("hlt");
    };
}


