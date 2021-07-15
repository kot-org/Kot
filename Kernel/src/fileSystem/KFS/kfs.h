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

    #define blockSize 0x10000

    #define GUIDData1 0x47A1ACC0
    #define GUIDData2 0x3B41 
    #define GUIDData3 0x2A53
    #define GUIDData4 0xF38D3D321F6D 

    struct Root{
        char        name[MaxName];
        uint64_t    firstBlockForFile;
        uint64_t    firstBlockFile;
        uint64_t    fid;
        uint64_t    lastBlockAllocated;
    }__attribute__((packed));

    struct KFSinfo{
        size_t      bitmapSizeByte;
        size_t      bitmapSizeBlock;
        uint64_t    bitmapPosition;
        size_t      BlockSize;
        size_t      numberOfBlock;
        Root        root;
        GUID        IsInit;
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
        uint64_t FID = 0;
        uint64_t ParentLocationBlock = 0; //location of the parent's header
        bool IsFile = false;
        uint64_t Version = 0;
    }__attribute__((packed));

    struct FileInfo{
        /* location info */
        uint64_t firstBlock;
        size_t size;  
        size_t FileBlockSize; //number of block 
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
        uint64_t BlockHeader;
        uint64_t firstBlockData;
        uint64_t numberFiles;
        size_t size;   
        size_t FileBlockSize; //number of block 
        char path[MaxPath];
        char name[MaxName];

        /* userRight */
        char password[MaxPassword];
        uint8_t userRight[MaxUserRight];
        GUID owner;

        /* time */
        TimeInfoFS timeInfoFS;

        uint64_t lastBlockRequested;
        uint64_t mode;

    }__attribute__((packed));

    struct PartitionNameAndGUID{
        char* name;
        GUID* UniquePartitionGUID;
    };

    class KFS;
    class File;
    class Folder;

    class File{
        public:
            KFS* Fs;
            void Read();
            void Write(size_t size, void* buffer);
            FileInfo* fileInfo;
            char* mode;
    };

    class Folder{
        public:
            KFS* Fs;
            FolderInfo* folderInfo;
    }; 

    class KFS{
        public:
            KFS(GPT::Partition* partition);

            void InitKFS();
            
            File* OpenFile(char* filePath);                        
            void Close(File* file);

            uint64_t Allocate(size_t size, Folder* folder);
            void Free(uint64_t Block, bool DeleteData);
            uint64_t RequestBlock();
            void LockBlock(uint64_t Block);  
            void UnlockBlock(uint64_t Block);  
            bool CheckBlock(uint64_t Block);  
            void GetBlockData(uint64_t Block, void* buffer);
            void SetBlockData(uint64_t Block, void* buffer);

            uint64_t mkdir(char* filePath, uint64_t mode);
            Folder* readdir(char* filePath);

            void flist(char *filename);

            File* fopen(char *filename, char *mode);            
            FileInfo* NewFile(char* filePath, Folder* folder);
            FolderInfo* OpenFolderInFolder(GPT::Partition* Partition, FolderInfo* FolderOpened, char* FolderName); 
            uint64_t GetFID();
            bool UpdatePartitionInfo();
            void UpdateFolderInfo(Folder* folder);

            GPT::Partition* globalPartition;
            KFSinfo* KFSPartitionInfo;
    }; 
}
