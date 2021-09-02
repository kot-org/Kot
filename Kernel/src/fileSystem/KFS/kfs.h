#pragma once

#include "../../lib/types.h"
#include "../gpt/gpt.h"
#include "../../lib/time.h"
#include "../../drivers/rtc/rtc.h"
#include "../../memory/heap/heap.h"

namespace FileSystem{
    #define MaxPath 512
    #define MaxName 256
    #define MaxPassword 256
    #define MaxUserRight 256   

    #define GUIDData1 0x47A1ACC0
    #define GUIDData2 0x3B41 
    #define GUIDData3 0x2A53
    #define GUIDData4 0xF38D3D321F6D 

    #define HeaderInfoPosition sizeof(ClusterHeader)
    #define FileInfoPosition sizeof(ClusterHeader) + sizeof(HeaderInfo)
    #define DataPosition sizeof(ClusterHeader) + sizeof(HeaderInfo) + sizeof(FileInfo)

    struct root{
        char        Name[MaxName];
        uint64_t    FirstClusterForFile;
        uint64_t    FirstClusterFile;
        uint64_t    FID;
        uint64_t    LastClusterAllocated;
    }__attribute__((packed));

    struct KFSinfo{
        size_t      BitmapSizeByte;
        size_t      BitmapSizeCluster;
        uint64_t    BitmapPosition;
        size_t      ClusterSize;
        size_t      NumberOfCluster;
        root        Root;
        uint64_t    IndexToAllocate;
        GUID        IsInit;
    }__attribute__((packed));



    struct ClusterHeader{
        uint64_t LastCluster;
        uint64_t NextCluster;
    }__attribute__((packed));

    struct timeInfoFS{
        Time CreateTime;
        Time ModifyTime;
    }__attribute__((packed));

    struct HeaderInfo{
        bool IsFile = false;
        uint64_t FID = 0;
        uint64_t ParentLocationCluster = 0; //location of the parent's header
        uint64_t Version = 0;
    }__attribute__((packed));

    struct FileInfo{
        /* location info */
        uint64_t LastClusterOfTheFile;
        uint64_t ClusterHeaderPostition;
        size_t BytesSize;  
        size_t ClusterSize; //number of Cluster 
        char Path[MaxPath];
        char Name[MaxName];

        /* userRight */
        char Password[MaxPassword];
        GUID Owner;

        /* time */
        timeInfoFS TimeInfoFS;

        uint64_t NextCluster;
    }__attribute__((packed));

    struct FolderInfo{
        /* location info */
        uint64_t ClusterHeaderPostion;
        uint64_t FirstClusterData;
        uint64_t numberFiles;
        size_t BytesSize;   
        size_t FileClusterSize; //number of Cluster 
        char Path[MaxPath];
        char Name[MaxName];

        /* userRight */
        char Password[MaxPassword];
        GUID Owner;

        /* time */
        timeInfoFS TimeInfoFS;

        uint64_t LastClusterRequested;
        uint64_t Mode;

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

        AllocatePartition* Allocate(size_t size, Folder* folder, uint64_t lastClusterRequested, bool GetAutoLastCluster);
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
        bool IsDirExist(char* filepath);

        File* fopen(char *filename, char *mode);            
        FileInfo* NewFile(char* filePath, Folder* folder);

        uint64_t GetFID();
        bool UpdatePartitionInfo();
        void UpdateFolderInfo(FolderInfo* folderInfo);
        void UpdateFileInfo(FileInfo* fileInfo);


        GPT::Partition* globalPartition;
        KFSinfo* KFSPartitionInfo;
    }__attribute__((packed));

    struct File{
        FileInfo* fileInfo;
        char* Mode;            
        KFS* kfs;
        uint64_t Read(uint64_t start, size_t size, void* buffer);
        uint64_t Write(uint64_t start, size_t size, void* buffer);
    }__attribute__((packed));

    struct Folder{
        FolderInfo* folderInfo;            
        KFS* kfs;
    }__attribute__((packed));
}
