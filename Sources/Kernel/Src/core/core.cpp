#include <core/core.h>

extern "C" void main(ukl_boot_structure_t* BootData){   
    /* load arch */
    ArchInfo_t* ArchInfo = arch_initialize(BootData);
    ArchInfo->revision = 1;

    /* load init file */
    initrd::File* InitFile = initrd::FindInitFile();
    
    if(InitFile != NULL){
        uintptr_t BufferInitFile = kmalloc(InitFile->size);
        initrd::Read(InitFile, BufferInitFile);
        kthread_t* mainthread;
        ELF::loadElf(BufferInitFile, PriviledgeDriver, &mainthread);
        arguments_t* InitParameters = (arguments_t*)kmalloc(sizeof(arguments_t));
        ThreadShareData_t DataInfo;
        GetDataToStartService(ArchInfo, mainthread, InitParameters, &DataInfo.Data, &DataInfo.Size);
        DataInfo.ParameterPosition = 0x0;
        globalTaskManager->Execthread(mainthread, mainthread, ExecutionTypeQueu, InitParameters, &DataInfo, NULL);
        kfree(InitParameters);
    }else{
        Error("Can't load initialization file");
    }
    kfree(ArchInfo);

    
    globalTaskManager->EnabledScheduler(CPU::GetAPICID());

    LaunchUserSpace();

    while(true){
        asm("hlt");
    };
}


