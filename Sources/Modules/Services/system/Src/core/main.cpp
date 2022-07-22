#include <kot/sys.h>
#include <kot/heap.h>
#include <kot/memory.h>
#include <core/main.h>

void ShareString(kthread_t self, char* str, uint64_t* clientAddress){
    SYS_ShareDataUsingStackSpace(self, (uint64_t)str, strlen(str) + 1, clientAddress);
}

extern "C" int main(struct KernelInfo* kernelInfo){
    Printlog("[SYSTEM] Initialization ...");
    
    kthread_t self;
    SYS_GetThreadKey(&self);
    ramfs::Parse(kernelInfo->ramfs.address, kernelInfo->ramfs.size);

    /* Load IPC */
    KotSpecificData.IPCHandler = IPCInitialize();
    
    /* Load services */
    ramfs::File* InitFile = ramfs::Find("Starter.cfg");
    
    if(InitFile != NULL) {
        char* BufferInitFile = (char*)malloc(InitFile->size + 1);
        ramfs::Read(InitFile, BufferInitFile);
        BufferInitFile[InitFile->size + 1] = NULL;
        char* app;
        uint8_t index = 0;
        char** ServicesInfo = strsplit(BufferInitFile, "\n");

        parameters_t* InitParameters = (parameters_t*)calloc(sizeof(parameters_t));

        for(uint64_t i = 0; ServicesInfo[i] != NULL; i++){
            char** ServiceInfo = strsplit(ServicesInfo[i], ", ");
            ramfs::File* ServiceFile = ramfs::Find(ServiceInfo[0]);
            if(ServiceFile != NULL && atoi(ServiceInfo[1]) >= PriviledgeDriver && atoi(ServiceInfo[1]) <= PriviledgeApp){
                uintptr_t BufferServiceFile = malloc(ServiceFile->size);
                ramfs::Read(ServiceFile, BufferServiceFile);
                kthread_t thread = NULL;
                ELF::loadElf(BufferServiceFile, (enum Priviledge)atoi(ServiceInfo[1]), NULL, &thread);
                free(BufferServiceFile);

                char** Parameters = (char**)malloc(sizeof(char*));

                InitParameters->Parameter0 = 1;
                ShareString(thread, ServiceInfo[0], (uint64_t*)&Parameters[0]);
                SYS_ShareDataUsingStackSpace(thread, (uint64_t)Parameters, sizeof(char*), &InitParameters->Parameter1);
                SYS_ShareDataUsingStackSpace(thread, (uint64_t)&kernelInfo->framebuffer, sizeof(framebuffer_t), &InitParameters->Parameter2);

                Sys_ExecThread(thread, InitParameters);
            }     
            freeSplit(ServiceInfo);
        }
        freeSplit(ServicesInfo);

        free(InitParameters);
    } else {
        Printlog("[SYSTEM] Starter.cfg not found");

        return KFAIL;
    }

    Printlog("[SYSTEM] All tasks in 'Starter.cfg' are loaded");
    Printlog("[SYSTEM] Service initialized successfully");
    return KSUCCESS;
}