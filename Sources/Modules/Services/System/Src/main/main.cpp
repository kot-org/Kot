#include <kot/sys.h>
#include <kot/heap.h>
#include <kot/memory.h>
#include <main/main.h>

void ShareString(kthread_t self, char* str, uint64_t* clientAddress){
    SYS_ShareDataUsingStackSpace(self, (uint64_t)str, strlen(str), clientAddress);
}

int main(struct KernelInfo* kernelInfo){
    kthread_t self;
    SYS_GetThreadKey(&self);
    ramfs::Parse(kernelInfo->ramfs.address, kernelInfo->ramfs.size);
    

    /* Load IPC */

    KotSpecificData.IPCHandler = NULL;
    
    /* Load services */

    ramfs::File* InitFile = ramfs::Find("Starter.cfg");
    
    if(InitFile != NULL){
        char* BufferInitFile = (char*)malloc(InitFile->size + 1);
        ramfs::Read(InitFile, BufferInitFile);
        BufferInitFile[InitFile->size + 1] = NULL;
        char* app;
        uint8_t index = 0;
        char** ServicesInfo = strsplit(BufferInitFile, "\n");
        for(uint64_t i = 0; ServicesInfo[i] != NULL; i++){
            char** ServiceInfo = strsplit(ServicesInfo[i], ", ");
            ramfs::File* ServiceFile = ramfs::Find(ServiceInfo[0]);
            if(ServiceFile != NULL && atoi(ServiceInfo[1]) >= PriviledgeDriver && atoi(ServiceInfo[1]) <= PriviledgeApp){
                uintptr_t BufferServiceFile = malloc(ServiceFile->size);
                ramfs::Read(ServiceFile, BufferServiceFile);
                kthread_t thread = NULL;
                ELF::loadElf(BufferServiceFile, (enum Priviledge)atoi(ServiceInfo[1]), NULL, &thread);
                free(BufferServiceFile);

                parameters_t* InitParameters = (parameters_t*)malloc(sizeof(parameters_t));

                char** mainArguments = (char**)malloc(KERNEL_INFO_SIZE * 2);
                InfoSlot Info;
                ShareString(thread, "FRAMEBUFFER", (uint64_t*)&mainArguments[0]);
                Info.size = sizeof(framebuffer_t);
                SYS_ShareDataUsingStackSpace(thread, (uint64_t)&kernelInfo->framebuffer, sizeof(framebuffer_t), (uint64_t*)&Info.address);
                SYS_ShareDataUsingStackSpace(self, (uint64_t)&Info, sizeof(InfoSlot), (uint64_t*)&mainArguments[1]);

                ShareString(thread, "RAMFS", (uint64_t*)&mainArguments[2]);
                Info.size = kernelInfo->ramfs.size;
                Info.address = kernelInfo->ramfs.address;
                SYS_ShareDataUsingStackSpace(self, (uint64_t)&Info, sizeof(InfoSlot), (uint64_t*)&mainArguments[3]);

                ShareString(thread, "MEMINFO", (uint64_t*)&mainArguments[4]);
                Info.size = sizeof(memoryInfo_t);
                Info.address = kernelInfo->memoryInfo;
                SYS_ShareDataUsingStackSpace(self, (uint64_t)&Info, sizeof(InfoSlot), (uint64_t*)&mainArguments[5]);

                ShareString(thread, "SMBIOS", (uint64_t*)&mainArguments[6]);
                Info.size = sizeof(uintptr_t);
                Info.address = kernelInfo->smbios;
                SYS_ShareDataUsingStackSpace(self, (uint64_t)&Info, sizeof(InfoSlot), (uint64_t*)&mainArguments[7]);

                ShareString(thread, "RSDP", (uint64_t*)&mainArguments[7]);
                Info.size = sizeof(uintptr_t);
                Info.address = kernelInfo->rsdp;
                SYS_ShareDataUsingStackSpace(self, (uint64_t)&Info, sizeof(InfoSlot), (uint64_t*)&mainArguments[7]);

                SYS_ShareDataUsingStackSpace(thread, (uint64_t)&mainArguments, KERNEL_INFO_SIZE * 2, &InitParameters->Parameter1);
                InitParameters->Parameter0 = KERNEL_INFO_SIZE;
                Sys_ExecThread(thread, InitParameters);

                free(InitParameters);
            }     
            freeSplit(ServiceInfo);
        }
        freeSplit(ServicesInfo);
    }
    return 1;
}