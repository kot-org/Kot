#pragma once
#include <stdint.h>
#include <stdlib.h>


#include "../gpt/gpt.h"
#include "../../lib/time.h"
#include "../../memory/heap.h"

namespace FileSystem{

    #define MaxPath 512
    #define MaxName 256
    #define MaxPassword 512
    #define MaxUserRight 256   

    #define blockSize 0x1000; 

    struct TimeInfoFS{
        Time CreateTime;
        Time ModifyTime;
    }__attribute__((packed));

    struct HeaderInfo{
        bool IsFile = false;
    }__attribute__((packed));

    struct FileInfo{
        /* location info */
        uint64_t firstByte;
        size_t size;           
        char path[MaxPath];
        char name[MaxName];

        /* userRight */
        char password[MaxPassword];
        uint8_t userRight[MaxUserRight];
        GUID owner;

        /* time */
        TimeInfoFS timeInfoFS;

        /* last folder */
        uint64_t lastHeaderInfoFolderByte;

        /* partition info */
        GUID UniquePartitionGUID;

        /* file/folder next to this */
        uint64_t topHeader;
        uint64_t bottomHeader;

    }__attribute__((packed));

    struct FolderInfo{
        /* location info */
        uint64_t firstByte;
        uint64_t numberFiles;
        size_t size;           
        char path[MaxPath];
        char name[MaxName];

        /* userRight */
        char password[MaxPassword];
        uint8_t userRight[MaxUserRight];
        GUID owner;

        /* time */
        TimeInfoFS timeInfoFS;

        /* next and last folder */
        uint64_t lastHeaderInfoFolderByte;

        /* partition info */
        GUID UniquePartitionGUID;

        /* file/folder next to this */
        uint64_t topHeader;
        uint64_t bottomHeader;

    }__attribute__((packed));

    struct PartitionNameAndGUID{
        char* name;
        GUID* UniquePartitionGUID;
    };

    class File{
        public:
            void Read();
            void Write(size_t size, void* buffer);
            FileInfo* fileInfo;
    };

    class KFS{
        public:
            KFS();

            File* OpenFile(char* filePath);            
            void Close(File* file);  
            
            FolderInfo* OpenFolderInFolder(GPT::Partition* Partition, FolderInfo* FolderOpened, char* FolderName); 

        private:
            PartitionNameAndGUID** IDPartitions;
    };  

}
