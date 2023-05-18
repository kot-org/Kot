#include "main.h"

#include <kot++/string.h>

using namespace std;

process_t proc;

extern "C" int main(KernelInfo* kernelInfo) {

    Printlog("[System] Initialization ...");

    proc = Sys_GetProcess();

    thread_t self = Sys_GetThread();

    // parse file system
    initrd::Parse((uintptr_t)kernelInfo->initrd.base, kernelInfo->initrd.size);

    // parse rsdp
    ParseRSDP(kernelInfo->Rsdp);

    // init interrupts
    InitializeInterrupts(kernelInfo);

    // load IPC
    KotSpecificData.UISDHandler = UISDInitialize(&KotSpecificData.UISDHandlerProcess);

    // load IPC server
    InitializeSrv(kernelInfo);

    // load starter file
    initrd::File* StarterFile = initrd::Find("Starter.json");

    if (StarterFile != NULL) {
        char* BufferStarterFile = (char*)calloc(StarterFile->size);
        initrd::Read(StarterFile, BufferStarterFile);
        
        JsonParser* parser = new JsonParser(BufferStarterFile);

        if (parser->getCode() == JSON_SUCCESS && parser->getValue()->getType() == JSON_ARRAY) {
            JsonArray* arr = (JsonArray*) parser->getValue();

            arguments_t* InitParameters = (arguments_t*)calloc(sizeof(arguments_t));

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
                    if(strcmp(file->Get(), "")) continue;
                    initrd::File* serviceFile = initrd::Find(file->Get());
                    if (serviceFile != NULL) {
                        uintptr_t bufferServiceFile = calloc(serviceFile->size);
                        initrd::Read(serviceFile, bufferServiceFile);
                        thread_t thread = NULL;
                        int32_t servicePriledge = 3;
                        if (priviledge != NULL) {
                            if (priviledge->getType() == JSON_NUMBER){ 
                                if (priviledge->Get() >= 1 && priviledge->Get() <= 3){
                                    servicePriledge = priviledge->Get();
                                }
                            }
                        }
                        ELF::loadElf(bufferServiceFile, (enum Priviledge)servicePriledge, NULL, &thread, "d0:", IsVFS);
                        free(bufferServiceFile);

                        size_t filenamelen = strlen(file->Get());
                        char** CharArray = (char**)malloc((sizeof(char*) * 0x1) + (sizeof(char) * filenamelen));
                        CharArray[0] = (char*)&CharArray[1];
                        memcpy(CharArray[0], file->Get(), filenamelen);

                        InitParameters->arg[0] = 1;
                        ShareDataWithArguments_t Data{
                            .Data = &CharArray,
                            .Size = (sizeof(char*) * 0x1) + (sizeof(char) * filenamelen),
                            .ParameterPosition = 0x1,
                        };
                        Sys_ExecThread(thread, InitParameters, ExecutionTypeQueu, &Data);
                    }
                }
            }
            free(InitParameters);
        } else { 
            Printlog("[System] Invalid Starter file's JSON body");
            return KFAIL;
        }

    } else {
        Printlog("[System] 'Starter.json' file not found");
        return KFAIL;
    }

    Printlog("[System] All tasks in 'Starter.json' are loaded");
    Printlog("[System] Service initialized successfully");

    return KSUCCESS;

}