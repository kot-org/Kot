#include <core/core.h>

extern "C" void main(void* boot)
{   
    /* load arch */
    KernelInfo* kernelInfo = arch_initialize(boot);

    /* load init file */
    ramfs::Parse(kernelInfo->ramfs->ramfsBase, kernelInfo->ramfs->Size);
    ramfs::File* InitFile = ramfs::FindInitFile();
    
    Parameters* InitParameters = (Parameters*)malloc(sizeof(Parameters));
    InitParameters->Parameter0 = (uint64_t)kernelInfo;
    if(InitFile != NULL){
        void* BufferInitFile = malloc(InitFile->size);
        ramfs::Read(InitFile, BufferInitFile);
        ELF::loadElf(BufferInitFile, 1, InitParameters);
    }else{
        globalLogs->Error("Can't load initialization file");
    }
    
    free(InitParameters);

    
    globalTaskManager->EnabledScheduler(CPU::GetAPICID());

    LaunchUserSpace();

    while(true){
        asm("hlt");
    };
}


