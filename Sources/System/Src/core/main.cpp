#include "main.h"

#include <kot++/string.h>

using namespace std;

kot_process_t proc;
void* FreeMemorySpaceAddress;


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
    FreeMemorySpaceAddress = KotSpecificData.FreeMemorySpace;

    KernelInfo* kernelInfo = (KernelInfo*)argv[0];

    kot_Printlog("[System] Initialization ...");

    proc = kot_Sys_GetProcess();

    kot_thread_t self = kot_Sys_GetThread();

    // parse file system
    initrd::Parse((void*)kernelInfo->initrd.base, kernelInfo->initrd.size);

    // parse rsdp
    ParseRSDP(kernelInfo->Rsdp);

    // init interrupts
    InitializeInterrupts(kernelInfo);

    // load IPC
    KotSpecificData.UISDHandler = UISDInitialize(&KotSpecificData.UISDHandlerProcess);

    // load IPC server
    InitializeSrv(kernelInfo);

    // load starter file
    initrd::InitrdFile* StarterFile = initrd::Find("Starter.json");

    if (StarterFile != NULL) {
        char* BufferStarterFile = (char*)calloc(1, StarterFile->size);
        initrd::Read(StarterFile, (void*)BufferStarterFile);
        
        JsonParser* parser = new JsonParser(BufferStarterFile);

        if (parser->getCode() == JSON_SUCCESS && parser->getValue()->getType() == JSON_ARRAY) {
            JsonArray* arr = (JsonArray*) parser->getValue();

            kot_arguments_t* InitParameters = (kot_arguments_t*)calloc(1, sizeof(kot_arguments_t));

            InitParameters->arg[2] = 1; // Disable shell
            
            for (uint64_t i = 0; i < arr->length(); i++) {
                JsonObject* service = (JsonObject*) arr->Get(i);
                JsonString* file = (JsonString*) service->Get("file");
                JsonNumber* priviledge = (JsonNumber*) service->Get("priviledge"); // default: 3
                JsonBoolean* active = (JsonBoolean*) service->Get("active"); // default: true
                JsonBoolean* vfs = (JsonBoolean*) service->Get("vfs"); // default: true

                bool IsVFS = true;

                if (active != NULL) {
                    if (active->getType() == JSON_BOOLEAN) {
                        if (active->Get() == false) {
                            continue;
                        }
                    }
                }

                if (vfs != NULL) {
                    if (vfs->getType() == JSON_BOOLEAN) {
                        IsVFS = vfs->Get();
                    }
                }
                
                if (file->getType() == JSON_STRING) {
                    if(!strcmp(file->Get(), "")) continue;
                    initrd::InitrdFile* serviceFile = initrd::Find(file->Get());
                    if (serviceFile != NULL) {
                        void* bufferServiceFile = (void*)calloc(1, serviceFile->size);
                        initrd::Read(serviceFile, bufferServiceFile);
                        kot_thread_t thread = NULL;
                        int32_t servicePriledge = 3;
                        if (priviledge != NULL) {
                            if (priviledge->getType() == JSON_NUMBER){ 
                                if (priviledge->Get() >= 1 && priviledge->Get() <= 3){
                                    servicePriledge = priviledge->Get();
                                }
                            }
                        }
                        ELF::loadElf(bufferServiceFile, NULL, (enum kot_Priviledge)servicePriledge, NULL, &thread, "d0:/", IsVFS, NULL);
                        free((void*)bufferServiceFile);

                        void* MainStackData;
                        size64_t SizeMainStackData;
                        char* Argv[] = {file->Get(), NULL};
                        char* Env[] = {NULL};
                        SetupStack(&MainStackData, &SizeMainStackData, 1, Argv, Env);

                        kot_ShareDataWithArguments_t Data{
                            .Data = MainStackData,
                            .Size = SizeMainStackData,
                            .ParameterPosition = 0x0,
                        };
                        kot_Sys_ExecThread(thread, InitParameters, ExecutionTypeQueu, &Data);
                    }
                }
            }
            free(InitParameters);
        } else { 
            kot_Printlog("[System] Invalid Starter file's JSON body");
            return KFAIL;
        }

    } else {
        kot_Printlog("[System] 'Starter.json' file not found");
        return KFAIL;
    }

    kot_Printlog("[System] All tasks in 'Starter.json' are loaded");
    kot_Printlog("[System] Service initialized successfully");

    return KSUCCESS;

}