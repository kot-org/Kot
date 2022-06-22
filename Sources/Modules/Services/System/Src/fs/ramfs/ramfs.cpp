#include <fs/ramfs/ramfs.h>

namespace ramfs{
    Info* info;
    void Parse(uintptr_t baseAddress, size_t size){
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

            if(strcmp(fileName, file->name)){
                return file;
            }

            cursor += sizeof(File) + file->size;
        }

        return NULL;
    }

    File* FindInitFile(){
        if(info->header->initfile == NULL) return NULL;
        File* file = (File*)(info->header->initfile + (uint64_t)info->baseAddress);
        return file;
    }

    bool Read(File* file, uintptr_t buffer){
        if(info == NULL) return false;
        uintptr_t fileData = (uintptr_t)((uint64_t)file + sizeof(File));
        memcpy(buffer, fileData, file->size);
        return true;
    }
} 