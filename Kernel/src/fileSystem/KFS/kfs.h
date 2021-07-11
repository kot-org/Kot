#pragma once
#include <stdint.h>
#include <stdlib.h>


#include "../gpt/gpt.h"
#include "../../lib/time.h"
#include "../../drivers/rtc/rtc.h"
#include "../../memory/heap.h"

namespace FileSystem{
    #define MaxPath 512
    #define MaxName 256
    #define MaxPassword 512
    #define MaxUserRight 256   

    #define blockSize 0x10000; 


    struct KFSinfo{
        bool        IsInit;
        size_t      bitmapSizeByte;
        size_t      bitmapSizeBlock;
        uint64_t    bitmapPosition;
        size_t      BlockSize;
        size_t      numberOfBlock;
        uint64_t    firstBlockFile;
    }__attribute__((packed));

    struct BlockHeader{
        uint64_t LastBlock;
        uint64_t NextBlock;
    }__attribute__((packed));

    struct TimeInfoFS{
        Time CreateTime;
        Time ModifyTime;
    }__attribute__((packed));

    struct HeaderInfo{
        bool IsFile = false;
    }__attribute__((packed));

    struct FileInfo{
        /* location info */
        uint64_t firstBlock;
        size_t size;  
        size_t FileBlockSize; 
        char path[MaxPath];
        char name[MaxName];

        /* userRight */
        char password[MaxPassword];
        uint8_t userRight[MaxUserRight];
        GUID owner;

        /* time */
        TimeInfoFS timeInfoFS;

    }__attribute__((packed));

    struct FolderInfo{
        /* location info */
        uint64_t firstBlock;
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
            char* mode;
    };

    class KFS{
        public:
            KFS(GPT::Partition* partition);

            void InitKFS();
            
            File* OpenFile(char* filePath);                        
            void Close(File* file);

            uint64_t Alloc(size_t size);
            void Free(uint64_t Block, bool DeleteData);
            uint64_t RequestBlock();
            void LockBlock(uint64_t Block);  
            void UnlockBlock(uint64_t Block);  
            bool CheckBlock(uint64_t Block);  
            void GetBlockData(uint64_t Block, void* buffer);
            void SetBlockData(uint64_t Block, void* buffer);

            File* fopen(char *filename, char *mode);
            FileInfo* NewFile(char* filePath);
            FolderInfo* OpenFolderInFolder(GPT::Partition* Partition, FolderInfo* FolderOpened, char* FolderName); 

        private:
            GPT::Partition* globalPartition;
            KFSinfo* KFSPartitionInfo;
    };  

}
