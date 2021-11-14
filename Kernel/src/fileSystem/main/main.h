#pragma once
#include "../../main/kernelInit.h"


class OSFileSystem{
    public:
        OSFileSystem(AHCI::PartitionNode* PartitionsList);
        AHCI::PartitionNode* partitions;

        AHCI::PartitionInfo* GetPartitionInfo(char* partitionName);

        //Main Fs functions
        uint64_t mkdir(char* filePath, uint64_t mode);
        FileSystem::Folder* readdir(char* filepath);

        void flist(char *filePath);
        bool IsDirExist(char* filepath);

        FileSystem::File* fopen(char *filepath, char *mode);         
        FileSystem::FileInfo* NewFile(char* filePath, FileSystem::Folder* folder);

        uint64_t rename(char* oldPath, char* newPath);

        uint64_t remove(char* filePath);  
};

extern OSFileSystem* fileSystem;