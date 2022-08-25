#include "main.h"

#include <kot++/string.h>

using namespace std;

process_t proc;

extern "C" int main(struct KernelInfo* kernelInfo) {

    Printlog("[System] Initialization ...");

    Sys_GetthreadKey(&proc);

    thread_t self;
    Sys_GetthreadKey(&self);

    ramfs::Parse(kernelInfo->Ramfs.address, kernelInfo->Ramfs.size);

    // load IPC
    KotSpecificData.UISDHandler = UISDInitialize(&KotSpecificData.UISDHandlerProcess);

    // load IPC server
    // InitializeSrv();

    // load starter file
    ramfs::File* StarterFile = ramfs::Find("Starter.json");

    if (StarterFile != NULL) {
        char* BufferStarterFile = (char*) calloc(StarterFile->size);
        ramfs::Read(StarterFile, BufferStarterFile);
        
        JsonParser* parser = new JsonParser(BufferStarterFile);

        if (parser->getCode() == JSON_SUCCESS && parser->getValue()->getType() == JSON_ARRAY) {
            JsonArray* arr = (JsonArray*) parser->getValue();

            arguments_t* InitParameters = (arguments_t*) calloc(sizeof(arguments_t));

            for (uint64_t i = 0; i < arr->length(); i++) {
                JsonObject* service = (JsonObject*) arr->get(i);
                JsonString* file = (JsonString*) service->get("file");
                JsonNumber* priviledge = (JsonNumber*) service->get("priviledge"); // default: 3
                JsonBoolean* active = (JsonBoolean*) service->get("active"); // default: true
                if (active != NULL) {
                    if (active->getType() == JSON_BOOLEAN) {
                        if (active->get() == false) {
                            continue;
                        }
                    }
                }
                if (file->getType() == JSON_STRING) {
                    if(strcmp(file->get(), "")) continue;
                    ramfs::File* serviceFile = ramfs::Find(file->get());
                    if (serviceFile != NULL) {
                        uintptr_t bufferServiceFile = calloc(serviceFile->size);
                        ramfs::Read(serviceFile, bufferServiceFile);
                        thread_t thread = NULL;
                        int32_t servicePriledge = 3;
                        if (priviledge != NULL) {
                            if (priviledge->getType() == JSON_NUMBER){ 
                                if (priviledge->get() >= 1 && priviledge->get() <= 3){
                                    servicePriledge = priviledge->get();
                                }
                            }
                        }
                        ELF::loadElf(bufferServiceFile, (enum Priviledge)servicePriledge, NULL, &thread);
                        free(bufferServiceFile);

                        size_t filenamelen = strlen(file->get());
                        char** CharArray = (char**)malloc((sizeof(char*) * 0x1) + (sizeof(char) * filenamelen));
                        CharArray[0] = (char*)&CharArray[1];
                        memcpy(CharArray[0], file->get(), filenamelen);

                        InitParameters->arg[0] = 1;
                        ShareDataWithArguments_t Data{
                            .Data = &CharArray,
                            .Size = sizeof(char*),
                            .ParameterPosition = 0x1,
                        };
                        Sys_Execthread(thread, InitParameters, ExecutionTypeQueu, &Data);
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