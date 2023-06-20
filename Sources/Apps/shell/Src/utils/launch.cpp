#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include <kot/sys.h>
#include <kot/uisd/srvs/system.h>

#include "system.h"

KResult SetupStack(void** Data, size64_t* Size, int argc, char** argv, char** envp){
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

    *Size = sizeof(void*) + (argc + 1) * sizeof(char*) + (envc + 1) * sizeof(char*) + args + envs;
    void* Buffer = malloc(*Size);
    
    void* StackDst = Buffer;

    *(void**)StackDst = (void*)argc;
    StackDst = (void*)((uint64_t)StackDst + sizeof(void*));

    void* OffsetDst = StackDst;
    StackDst = (void*)((uint64_t)StackDst + (argc + 1) * sizeof(char*) + (envc + 1) * sizeof(char*));

    for(int i = 0; i < argc; i++){
        *((void**)OffsetDst) = (void*)((uint64_t)StackDst - (uint64_t)Buffer);
        OffsetDst = (void*)((uint64_t)OffsetDst + sizeof(void*));
        strcpy((char*)StackDst, argv[i]);
        StackDst = (void*)((uint64_t)StackDst + strlen(argv[i]) + 1); // Add NULL char at the end
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



int main(int argc, char* argv[]) {
    if(argc < 2){
        printf("Usage : launch <executable-path>\n");
        return EXIT_FAILURE;
    }

    char* Prefix = "d1:Bin/";
    size_t PathSize = strlen(Prefix) + strlen(argv[1]) + 1;
    char* Path = (char*)calloc(PathSize, sizeof(char));

    strcat(Path, Prefix);
    strcat(Path, argv[1]);

    kot_srv_system_callback_t* Callback = kot_Srv_System_LoadExecutable(PriviledgeApp, Path, true);

    if(Callback->Status == KSUCCESS){
        void* MainStackData;
        size64_t SizeMainStackData;
        char* Argv[] = {Path, NULL};
        char* Env[] = {NULL};
        SetupStack(&MainStackData, &SizeMainStackData, 1, Argv, Env);

        kot_ShareDataWithArguments_t Data{
            .Data = MainStackData,
            .Size = SizeMainStackData,
            .ParameterPosition = 0x0,
        };
        kot_arguments_t Parameters;
        Parameters.arg[2] = 0;

        assert(kot_Sys_ExecThread((kot_thread_t)Callback->Data, &Parameters, ExecutionTypeQueu, &Data) == KSUCCESS);
        free(MainStackData);
        free(Callback);
    }else{
        free(Callback);
        printf("File not found : '%s'\n", Path);
    }

    return EXIT_SUCCESS;
}