#include <system/system.h>

KResult SetupStack(uintptr_t* Data, size64_t* Size, int argc, char** argv, char** envp){
    size64_t args = 0;
    for(int i = 0; i < argc; i++){
        args += strlen(argv[i]) + 1; // Add NULL char at the end
    }
    size64_t envc = 0;
    size64_t envs = 0;
    auto ev = envp;
	while(*ev){
		envc++;
        envs += strlen(*ev) + 1; // Add NULL char at the end
	}

    *Size = sizeof(uintptr_t) + (argc + 1) * sizeof(char*) + (envc + 1) * sizeof(char*) + args + envs;
    uintptr_t Buffer = malloc(*Size);
    
    uintptr_t StackDst = Buffer;

    *(uintptr_t*)StackDst = (uintptr_t)argc;
    StackDst = (uintptr_t)((uint64_t)StackDst + sizeof(uintptr_t));

    uintptr_t OffsetDst = StackDst;
    StackDst = (uintptr_t)((uint64_t)StackDst + (argc + 1) * sizeof(char*) + (envc + 1) * sizeof(char*));

    for(int i = 0; i < argc; i++){
        *((uintptr_t*)OffsetDst) = (uintptr_t)((uint64_t)StackDst - (uint64_t)Buffer);
        OffsetDst = (uintptr_t)((uint64_t)OffsetDst + sizeof(uintptr_t));
        strcpy((char*)StackDst, argv[i]);
        StackDst = (uintptr_t)((uint64_t)StackDst + strlen(argv[i]) + 1); // Add NULL char at the end
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

KResult ExecuteSystemAction(uint64_t PartitonID){
    // Load filesystem handler
    if(!KotSpecificData.VFSHandler){
        srv_storage_callback_t* Callback = Srv_Storage_VFSLoginApp(Sys_GetProcess(), FS_AUTHORIZATION_HIGH, Storage_Permissions_Admin | Storage_Permissions_Read | Storage_Permissions_Write | Storage_Permissions_Create, "d0:", true);
        KotSpecificData.VFSHandler = Callback->Data;
        free(Callback);
    }

    // TODO certify disk as system
    bool IsSystem = true;
    if(IsSystem){
        KResult Status = KFAIL;

        char DiskNumberBuffer[20];
        char* DiskNumber = itoa(PartitonID, (char*)&DiskNumberBuffer, 10);

        StringBuilder* SystemDataPathBuilder = new StringBuilder("d");
        SystemDataPathBuilder->append(DiskNumberBuffer);
        SystemDataPathBuilder->append(":Kot/System/Starter.json");
        char* SystemDataPath = SystemDataPathBuilder->toString();
        delete SystemDataPathBuilder;
        file_t* SystemDataFile = fopen(SystemDataPath, "r");
        if(SystemDataFile){
            fseek(SystemDataFile, 0, SEEK_END);
            size_t SystemDataFileSize = ftell(SystemDataFile);
            fseek(SystemDataFile, 0, SEEK_SET);

            char* BufferSystemDataFile = (char*)malloc(SystemDataFileSize);
            fread(BufferSystemDataFile, SystemDataFileSize, 1, SystemDataFile);

            JsonParser* Parser = new JsonParser(BufferSystemDataFile);


            if(Parser->getCode() == JSON_SUCCESS && Parser->getValue()->getType() == JSON_ARRAY){
                JsonArray* Array = (JsonArray*) Parser->getValue();

                arguments_t* InitParameters = (arguments_t*)calloc(sizeof(arguments_t));

                for(uint64_t i = 0; i < Array->length(); i++){
                    JsonObject* Service = (JsonObject*) Array->Get(i);
                    JsonString* File = (JsonString*) Service->Get("file");
                    JsonNumber* Priviledge = (JsonNumber*) Service->Get("priviledge"); // default: 3
                    
                    if (File->getType() == JSON_STRING) {
                        if(strcmp(File->Get(), "")) continue;
                        int32_t ServicePriledge = 3;
                        if(Priviledge != NULL){
                            if(Priviledge->getType() == JSON_NUMBER){ 
                                if(Priviledge->Get() >= 1 && Priviledge->Get() <= 3){
                                    ServicePriledge = Priviledge->Get();
                                }
                            }
                        }

                        StringBuilder* ServicePathBuilder = new StringBuilder("d");
                        ServicePathBuilder->append(DiskNumberBuffer);
                        ServicePathBuilder->append(":");
                        ServicePathBuilder->append(File->Get());
                        char* FilePath = ServicePathBuilder->toString();
                        delete ServicePathBuilder;

                        srv_system_callback_t* Callback = Srv_System_LoadExecutable(Priviledge->Get(), FilePath, true);

                        uintptr_t MainStackData;
                        size64_t SizeMainStackData;
                        char* Argv[] = {FilePath, NULL};
                        char* Env[] = {NULL};
                        SetupStack(&MainStackData, &SizeMainStackData, 1, Argv, Env);
                        free(FilePath);

                        ShareDataWithArguments_t Data{
                            .Data = MainStackData,
                            .Size = SizeMainStackData,
                            .ParameterPosition = 0x0,
                        };
                        Sys_ExecThread((thread_t)Callback->Data, InitParameters, ExecutionTypeQueu, &Data);
                        free(MainStackData);
                        free(Callback);
                    }
                }
                free(InitParameters);
                Status = KSUCCESS;
            }
            delete Parser;
            free(SystemDataFile);
            free(BufferSystemDataFile);
        }
        free(SystemDataPath);

        return Status;
    }

    return KNOTALLOW;
}