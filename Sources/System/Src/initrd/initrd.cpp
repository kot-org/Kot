#include <initrd/initrd.h>
#include <stdio.h>

namespace initrd {

    Info* info;

    void Parse(void* baseAddress, size64_t size) {
        if (size == NULL) return;
        info = (Info*) malloc(sizeof(Info));
        info->baseAddress = baseAddress;
        info->size = size;
        info->header = (Header*) baseAddress;
    }

    InitrdFile* Find(char* fileName) {
        if (info == NULL) return NULL;
        uint64_t cursor = sizeof(Header);
        for (uint64_t i = 0; i < info->header->filenumber; i++){
            InitrdFile* file = (InitrdFile*) (cursor + (uint64_t)info->baseAddress);
            if (!strcmp(fileName, file->name)) {
                return file;
            }
            cursor += sizeof(InitrdFile) + file->size;
        }
        return NULL;
    }

    InitrdFile* FindInitFile() {
        if (info->header->initfile == NULL) return NULL;
        InitrdFile* file = (InitrdFile*) (info->header->initfile + (uint64_t) info->baseAddress);
        return file;
    }

    bool Read(InitrdFile* file, void* buffer) {
        if (info == NULL) return false;
        void* fileData = (void*)((uint64_t)file + sizeof(InitrdFile));
        memcpy((void*)buffer, fileData, file->size);
        return true;
    }

    void* Read(InitrdFile* file) {
        if (info == NULL) return NULL;
        void* fileData = (void*) ((uint64_t)file + sizeof(InitrdFile));
        return fileData;
    }

} 
