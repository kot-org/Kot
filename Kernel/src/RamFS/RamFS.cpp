#include "RamFS.h"

namespace RamFS{
    Info* info;
    void Parse(void* baseAddress, size_t size){
        if(size == NULL) return;
        info = (Info*)malloc(sizeof(Info));
        info->baseAddress = baseAddress;
        info->size = size;
        info->header = (Header*)baseAddress;
    }

    File* Find(char* fileName){
        if(info == NULL) return NULL;
        uint64_t cursor = sizeof(Header);
        for(int i = 0; i < info->header->filenumber; i++){
            File* file = (File*)(cursor + (uint64_t)info->baseAddress);
            globalLogs->Successful("%s", file->name);
            if(strcmp(fileName, file->name)){
                return file;
            }

            cursor += sizeof(File) + file->size;
        }

        return NULL;
    }

    bool ReadInitFile(void* buffer){
        return Read((File*)info->header->initfile, buffer);
    }

    bool Read(File* address, void* buffer){
        if(info == NULL) return false;
        File* file = (File*)(address + (uint64_t)info->baseAddress);
        void* fileData = file + sizeof(File);
        memcpy(buffer, fileData, file->size);
    }

}