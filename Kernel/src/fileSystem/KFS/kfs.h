#pragma once

#include "../../lib/types.h"
#include "../gpt/gpt.h"
#include "../../lib/time.h"
#include "../../drivers/rtc/rtc.h"
#include "../../memory/heap/heap.h"

namespace FileSystem{
    #define MaxPath 512
    #define MaxName 256
    #define MaxPassword 512
    #define MaxUserRight 256   

    #define GUIDData1 0x47A1ACC0
    #define GUIDData2 0x3B41 
    #define GUIDData3 0x2A53
    #define GUIDData4 0xF38D3D321F6D 

    struct Root{
        char        name[MaxName];
        uint64_t    firstClusterForFile;
        uint64_t    firstClusterFile;
        uint64_t    fid;
        uint64_t    lastClusterAllocated;
    }__attribute__((packed));

    struct KFSinfo{
        size_t      bitmapSizeByte;
        size_t      bitmapSizeCluster;
        uint64_t    bitmapPosition;
        size_t      ClusterSize;
        size_t      numberOfCluster;
        Root        root;
        GUID        IsInit;
    }__attribute__((packed));



    struct ClusterHeader{
        uint64_t LastCluster;
        uint64_t NextCluster;
    }__attribute__((packed));

    struct TimeInfoFS{
        Time CreateTime;
        Time ModifyTime;
    }__attribute__((packed));

    struct HeaderInfo{
        uint64_t FID = 0;
        uint64_t ParentLocationCluster = 0; //location of the parent's header
        bool IsFile = false;
        uint64_t Version = 0;
        uint64_t LastClusterAllocated = 0;
    }__attribute__((packed));

    struct FileInfo{
        /* location info */
        uint64_t ClusterHeader;
        uint64_t firstClusterData;
        uint64_t lastCluster;
        size_t size;  
        size_t FileClusterSize; //number of Cluster 
        size_t FileClusterData; //number of Cluster of data
        char path[MaxPath];
        char name[MaxName];

        /* userRight */
        char password[MaxPassword];
        GUID owner;

        /* time */
        TimeInfoFS timeInfoFS;

    }__attribute__((packed));

    struct FolderInfo{
        /* location info */
        uint64_t ClusterHeader;
        uint64_t firstClusterData;
        uint64_t numberFiles;
        size_t size;   
        size_t FileClusterSize; //number of Cluster 
        char path[MaxPath];
        char name[MaxName];

        /* userRight */
        char password[MaxPassword];
        GUID owner;

        /* time */
        TimeInfoFS timeInfoFS;

        uint64_t lastClusterRequested;
        uint64_t mode;

    }__attribute__((packed));

    struct AllocatePartition{
        uint64_t FirstCluster;
        uint64_t LastCluster;
    }__attribute__((packed));

    struct PartitionNameAndGUID{
        char* name;
        GUID* UniquePartitionGUID;
    };

    class File;
    class Folder;

    struct KFS{         
        KFS(GPT::Partition* partition);

        void InitKFS();
            
        File* OpenFile(char* filePath);                        
        void Close(File* file);

        AllocatePartition* Allocate(size_t size, Folder* folder, uint64_t lastClusterRequested);
        void Free(uint64_t Cluster, bool DeleteData);
        uint64_t RequestCluster();
        void LockCluster(uint64_t Cluster);  
        void UnlockCluster(uint64_t Cluster);  
        bool CheckCluster(uint64_t Cluster);  
        void GetClusterData(uint64_t Cluster, void* buffer);
        void SetClusterData(uint64_t Cluster, void* buffer);

        uint64_t mkdir(char* filePath, uint64_t mode);
        Folder* readdir(char* filePath);

        void flist(char *filename);

        File* fopen(char *filename, char *mode);            
        FileInfo* NewFile(char* filePath, Folder* folder);
        FolderInfo* OpenFolderInFolder(GPT::Partition* Partition, FolderInfo* FolderOpened, char* FolderName); 
        uint64_t GetFID();
        bool UpdatePartitionInfo();
        void UpdateFolderInfo(Folder* folder);
        void UpdateFileInfo(FileInfo* fileInfo);

        GPT::Partition* globalPartition;
        KFSinfo* KFSPartitionInfo;
    }; 

    struct File{
        uint64_t Read(uint64_t start, size_t size, void* buffer);
        uint64_t Write(uint64_t start, size_t size, void* buffer);
        FileInfo* fileInfo;
        char* mode;            
        KFS* kfs;
    };
    class Folder{
        public:
            FolderInfo* folderInfo;            
        private:
            KFS* kfs;
    };
}
