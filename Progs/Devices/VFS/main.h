#pragma once
#include "../../main/kernelInit.h"


class OSFileSystem{
    public:
        OSFileSystem(AHCI::PartitionNode* PartitionsList);
        AHCI::PartitionNode* partitions;

        AHCI::PartitionInfo* GetPartitionInfo(char* partitionName);

        //Main Fs functions
        uint64_t mkdir(char* filePath, uint64_t mode);
        bool readdir(char* filepath, FileSystem::Folder* folder);

        void flist(char *filePath);
        bool IsDirExist(char* filepath);

        bool fopen(char* filePath, char* mode, FileSystem::File* file);

        uint64_t rename(char* oldPath, char* newPath);

        uint64_t remove(char* filePath);  
};

extern OSFileSystem* fileSystem;