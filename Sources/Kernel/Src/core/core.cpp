#include <core/core.h>

struct arg_t{
    void* Data;
    size64_t Size;
};

KResult SetupStack(void** Data, size64_t* Size, int argc, arg_t* argv, char** envp){
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

    *Size = sizeof(void*) + (argc + 1) * sizeof(char*) + (envc + 1) * sizeof(char*) + args + envs;
    void* Buffer = kmalloc(*Size);
    
    void* StackDst = Buffer;

    *(void**)StackDst = (void*)argc;
    StackDst = (void*)((uint64_t)StackDst + sizeof(void*));

    void* OffsetDst = StackDst;
    StackDst = (void*)((uint64_t)StackDst + (argc + 1) * sizeof(char*) + (envc + 1) * sizeof(char*));

    for(int i = 0; i < argc; i++){
        *((void**)OffsetDst) = (void*)((uint64_t)StackDst - (uint64_t)Buffer);
        OffsetDst = (void*)((uint64_t)OffsetDst + sizeof(void*));
        memcpy((char*)StackDst, argv[i].Data, argv[i].Size);
        StackDst = (void*)((uint64_t)StackDst + argv[i].Size);
    }

    // Null argument
    *(void**)OffsetDst = NULL;
    OffsetDst = (void*)((uint64_t)OffsetDst + sizeof(void*));

    for(int i = 0; i < envc; i++){
        *(void**)OffsetDst = (void*)((uint64_t)StackDst - (uint64_t)Buffer);
        OffsetDst = (void*)((uint64_t)OffsetDst + sizeof(void*));
        strcpy((char*)StackDst, envp[i]);
        StackDst = (void*)((uint64_t)StackDst + strlen(envp[i]) + 1); // Add NULL char at the end
    }
    // Null argument
    *(void**)OffsetDst = NULL;

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
        void* BufferInitFile = kmalloc(InitFile->size);
        initrd::Read(InitFile, BufferInitFile);
        kthread_t* mainthread;
        ELF::loadElf(BufferInitFile, PriviledgeDriver, &mainthread);
        arguments_t* InitParameters = (arguments_t*)kmalloc(sizeof(arguments_t));

        size64_t Size;
        void* Data;
        GetDataToStartService(ArchInfo, mainthread, InitParameters, &Data, &Size);

        void* MainStackData;
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

        kfree(Data);
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


