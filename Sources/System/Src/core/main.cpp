#include "main.h"

#include <kot/sys.h>
#include <kot/heap.h>
#include <kot/memory.h>

#include <kot++/json.h>

using namespace std;

process_t proc;

void ShareString(thread_t self, char* str, uint64_t* clientAddress){
    SYS_ShareDataUsingStackSpace(self, (uint64_t)str, strlen(str) + 1, clientAddress);
}

extern "C" int main(struct KernelInfo* kernelInfo) {

    Printlog("[System] Initialization ...");

    Sys_GetthreadKey(&proc);
    
    thread_t self;
    Sys_GetthreadKey(&self);

    ramfs::Parse(kernelInfo->ramfs.address, kernelInfo->ramfs.size);

    // load IPC
    KotSpecificData.UISDHandler = UISDInitialize(&KotSpecificData.UISDHandlerProcess);

    // load starter file
    ramfs::File* StarterFile = ramfs::Find("Starter.json");

    if (StarterFile != NULL) {

        char* BufferStarterFile = (char*) calloc(StarterFile->size);
        ramfs::Read(StarterFile, BufferStarterFile);
        
        JsonParser* parser = new JsonParser(BufferStarterFile);

        if (parser->getCode() == JSON_SUCCESS && parser->getValue()->getType() == JSON_ARRAY) {

            // todo delete testClass (for JVM)
            ramfs::File* testClass = ramfs::Find("Test.class");
            uintptr_t testClassBuffer = (uintptr_t) calloc(testClass->size);
            ramfs::Read(testClass, testClassBuffer);

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
                    if (strcmp(file->get(), "")) { continue; }
                    ramfs::File* serviceFile = ramfs::Find(file->get());
                    if (serviceFile != NULL) {
                        uintptr_t bufferServiceFile = calloc(serviceFile->size);
                        ramfs::Read(serviceFile, bufferServiceFile);
                        thread_t thread = NULL;
                        int32_t servicePriledge = 3;
                        if (priviledge == NULL) {
                            if (priviledge->getType() == JSON_NUMBER) {
                                if (priviledge->get() >= 1 && priviledge->get() <= 3) {
                                    servicePriledge = priviledge->get();
                                }
                            }
                        }
                        ELF::loadElf(bufferServiceFile, (enum Priviledge) servicePriledge, NULL, &thread);
                        free(bufferServiceFile);
                        char** Parameters = (char**) calloc(sizeof(char*));
                        InitParameters->arg[0] = 1;
                        ShareString(thread, file->get(), (uint64_t*) &Parameters[0]);
                        SYS_ShareDataUsingStackSpace(thread, (uint64_t) Parameters, sizeof(char*), &InitParameters->arg[1]);
                        SYS_ShareDataUsingStackSpace(thread, (uint64_t) &kernelInfo->framebuffer, sizeof(framebuffer_t), &InitParameters->arg[2]);
                        SYS_ShareDataUsingStackSpace(thread, (uint64_t) testClassBuffer, testClass->size + 1, &InitParameters->arg[3]);
                        Sys_Execthread(thread, InitParameters, ExecutionTypeQueu, NULL);
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