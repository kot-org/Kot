#include "kernelIPC.h"

namespace KernelIPC{
    TaskNode* parent = NULL;

    void Initialize(){
        parent = globalTaskManager->AddTask(false, false, 0, "Kernel device");
        CreatTask((void*)LogHandler, 0);
    }

    void CreatTask(void* EntryPoint, uint16_t Index){
        if(parent == NULL) return;
        DeviceTaskAdressStruct DeviceAdress;
        DeviceAdress.type = 0;
        DeviceAdress.L1 = 0;
        DeviceAdress.L2 = 0;
        DeviceAdress.L3 = Index / 512;
        DeviceAdress.FunctionID = Index % 512;
        globalTaskManager->CreatSubTask(parent, EntryPoint, &DeviceAdress);
    }

    //IPC functions

    void LogHandler(uint64_t type, char* str){
        switch(type){
            case 0:
                globalLogs->Message(str);
                break;
            case 1:
                globalLogs->Error(str);
                break;
            case 2:
                globalLogs->Warning(str);
                break;
            case 3:
                globalLogs->Successful(str);
                break;
        }

        DoSyscall(ReturnSyscall, 0, 1, 0, 0, 0, 0);
    }

    void ReadFile(FileSystem::File* file, uint64_t start, size_t size, void* buffer){
        uint64_t returnValue = file->Read(start, size, buffer);
        DoSyscall(ReturnSyscall, 0, returnValue, 0, 0, 0, 0);
    } 

    void WriteFile(FileSystem::File* file, uint64_t start, size_t size, void* buffer){
        uint64_t returnValue = file->Write(start, size, buffer);
        DoSyscall(ReturnSyscall, 0, returnValue, 0, 0, 0, 0);        
    } 

    void OpenFile(char* filePath, char* mode, FileSystem::File* file){
        uint64_t returnValue = (uint64_t)fileSystem->fopen(filePath, mode, file);
        DoSyscall(ReturnSyscall, 0, returnValue, 0, 0, 0, 0);
    }
}

