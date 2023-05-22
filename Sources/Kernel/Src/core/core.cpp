#include <core/core.h>

struct arg_t{
    uintptr_t Data;
    size64_t Size;
};

KResult SetupStack(uintptr_t* Data, size64_t* Size, int argc, arg_t* argv, char** envp){
    size64_t args = 0;
    for(int i = 0; i < argc; i++){
        args += argv[i].Size;
    }
    size64_t envc = 0;
    size64_t envs = 0;
    auto ev = envp;
	while(*ev){
		envc++;
        envs += strlen(*ev) + 1; // Add NULL char at the end
	}

    *Size = sizeof(uintptr_t) + (argc + 1) * sizeof(char*) + (envc + 1) * sizeof(char*) + args + envs;
    uintptr_t Buffer = kmalloc(*Size);
    
    uintptr_t StackDst = Buffer;

    *(uintptr_t*)StackDst = (uintptr_t)argc;
    StackDst = (uintptr_t)((uint64_t)StackDst + sizeof(uintptr_t));

    uintptr_t OffsetDst = StackDst;
    StackDst = (uintptr_t)((uint64_t)StackDst + (argc + 1) * sizeof(char*) + (envc + 1) * sizeof(char*));

    for(int i = 0; i < argc; i++){
        *((uintptr_t*)OffsetDst) = (uintptr_t)((uint64_t)StackDst - (uint64_t)Buffer);
        OffsetDst = (uintptr_t)((uint64_t)OffsetDst + sizeof(uintptr_t));
        memcpy((char*)StackDst, argv[i].Data, argv[i].Size);
        StackDst = (uintptr_t)((uint64_t)StackDst + argv[i].Size);
    }

    // Null argument
    *(uintptr_t*)OffsetDst = NULL;
    OffsetDst = (uintptr_t)((uint64_t)OffsetDst + sizeof(uintptr_t));

    for(int i = 0; i < envc; i++){
        *(uintptr_t*)OffsetDst = (uintptr_t)((uint64_t)StackDst - (uint64_t)Buffer);
        OffsetDst = (uintptr_t)((uint64_t)OffsetDst + sizeof(uintptr_t));
        strcpy((char*)StackDst, envp[i]);
        StackDst = (uintptr_t)((uint64_t)StackDst + strlen(envp[i]) + 1); // Add NULL char at the end
    }
    // Null argument
    *(uintptr_t*)OffsetDst = NULL;

    *Data = Buffer;

    return KSUCCESS;
}


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

        size64_t Size;
        uintptr_t Data;
        GetDataToStartService(ArchInfo, mainthread, InitParameters, &Data, &Size);

        uintptr_t MainStackData;
        size64_t SizeMainStackData;

        arg_t Argv[1];
        Argv[0].Data = Data;
        Argv[0].Size = Size;

        char* Env[1];
        Env[0] = NULL;

        SetupStack(&MainStackData, &SizeMainStackData, 1, Argv, Env);

        ThreadShareData_t DataInfo;
        DataInfo.ParameterPosition = 0x0;
        DataInfo.Data = MainStackData;
        DataInfo.Size = SizeMainStackData;

        InitParameters->arg[2] = 1; // Disable shell

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


