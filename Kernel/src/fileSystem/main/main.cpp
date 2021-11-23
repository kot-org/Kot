#pragma once
#include "main.h"

char* PartitionName[25];
OSFileSystem* fileSystem;

OSFileSystem::OSFileSystem(AHCI::PartitionNode* PartitionsList){
    PartitionName[0] = "Alpha"; //System Disk
    PartitionName[1] = "Beta";
    PartitionName[2] = "Gamma";
    PartitionName[3] = "Delta";
    PartitionName[4] = "Epsilon";
    PartitionName[5] = "Zeta";
    PartitionName[6] = "Eta";
    PartitionName[7] = "Theta";
    PartitionName[8] = "Iota";
    PartitionName[9] = "Kappa";
    PartitionName[10] = "Lambda";


    partitions = (AHCI::PartitionNode*)malloc(sizeof(AHCI::PartitionNode));
    memcpy(partitions, PartitionsList, sizeof(AHCI::PartitionNode));
    AHCI::PartitionNode* list = partitions;
    GUID* guid = GPT::GetSystemGUIDPartitionType();
    int i = 1;
    while(list != NULL){
        if(list->Content.PartitionInfo->PartitionTypeGUID.Data1 == guid->Data1 &&
            list->Content.PartitionInfo->PartitionTypeGUID.Data2 == guid->Data2 &&
            list->Content.PartitionInfo->PartitionTypeGUID.Data3 == guid->Data3 &&
            list->Content.PartitionInfo->PartitionTypeGUID.Data4 == guid->Data4){
            list->Content.PartitionName = PartitionName[0];
        }else{
            list->Content.PartitionName = PartitionName[i];
            i++;
        }
        list = list->Next;
    }

    fileSystem = this;
}


AHCI::PartitionInfo* OSFileSystem::GetPartitionInfo(char* partitionName){
    AHCI::PartitionNode* list = partitions;
    GUID* guid = GPT::GetSystemGUIDPartitionType();
    while(list != NULL){
        if(strcmp(list->Content.PartitionName, partitionName)){
            return &list->Content;
        }
        list = list->Next;
    }
}

//TODO make read/write delete for other FS

uint64_t OSFileSystem::mkdir(char* filePath, uint64_t mode){
    Node* filePathSplit = split(filePath, ":/");
    AHCI::PartitionInfo* info = GetPartitionInfo((char*)filePathSplit->GetNode(0)->data);
    if(strcmp(info->FSSignature, "KOTFS")){
        FileSystem::KFS* Fs = (FileSystem::KFS*)info->FSData;
        return Fs->mkdir((char*)filePathSplit->GetNode(1)->data, mode);
    }
    return 0;
}

FileSystem::Folder* OSFileSystem::readdir(char* filePath){
    Node* filePathSplit = split(filePath, ":/");
    AHCI::PartitionInfo* info = GetPartitionInfo((char*)filePathSplit->GetNode(0)->data);
    if(strcmp(info->FSSignature, "KOTFS")){
        FileSystem::KFS* Fs = (FileSystem::KFS*)info->FSData;
        return Fs->readdir((char*)filePathSplit->GetNode(1)->data);
    }
    return 0;
}

void OSFileSystem::flist(char *filepath){
    Node* filePathSplit = split(filepath, ":/");
    AHCI::PartitionInfo* info = GetPartitionInfo((char*)filePathSplit->GetNode(0)->data);
    if(strcmp(info->FSSignature, "KOTFS")){
        FileSystem::KFS* Fs = (FileSystem::KFS*)info->FSData;
        Fs->flist((char*)filePathSplit->GetNode(1)->data);
    }
}

bool OSFileSystem::IsDirExist(char* filepath){
    Node* filePathSplit = split(filepath, ":/");
    AHCI::PartitionInfo* info = GetPartitionInfo((char*)filePathSplit->GetNode(0)->data);
    if(strcmp(info->FSSignature, "KOTFS")){
        FileSystem::KFS* Fs = (FileSystem::KFS*)info->FSData;
        Fs->IsDirExist((char*)filePathSplit->GetNode(1)->data);
    }
    return 0;
}

FileSystem::File* OSFileSystem::fopen(char* filePath, char* mode){
    Node* filePathSplit = split(filePath, ":/");
    AHCI::PartitionInfo* info = GetPartitionInfo((char*)filePathSplit->GetNode(0)->data);
    if(strcmp(info->FSSignature, "KOTFS")){
        FileSystem::KFS* Fs = (FileSystem::KFS*)info->FSData;
        return Fs->fopen((char*)filePathSplit->GetNode(1)->data, mode);
    }
    return 0;
}

FileSystem::FileInfo* OSFileSystem::NewFile(char* filePath, FileSystem::Folder* folder){
    Node* filePathSplit = split(filePath, ":/");
    AHCI::PartitionInfo* info = GetPartitionInfo((char*)filePathSplit->GetNode(0)->data);
    if(strcmp(info->FSSignature, "KOTFS")){
        FileSystem::KFS* Fs = (FileSystem::KFS*)info->FSData;
        return Fs->NewFile((char*)filePathSplit->GetNode(1)->data, folder);
    }
    return 0;
}

//TODO move file between two partitions
uint64_t OSFileSystem::rename(char* oldPath, char* newPath){
    Node* filePathSplit = split(oldPath, ":/");
    AHCI::PartitionInfo* info = GetPartitionInfo((char*)filePathSplit->GetNode(0)->data);
    if(strcmp(info->FSSignature, "KOTFS")){
        FileSystem::KFS* Fs = (FileSystem::KFS*)info->FSData;
        return Fs->rename((char*)filePathSplit->GetNode(1)->data, newPath);
    }
    return 0;
}

uint64_t OSFileSystem::remove(char* filePath){
    Node* filePathSplit = split(filePath, ":/");
    AHCI::PartitionInfo* info = GetPartitionInfo((char*)filePathSplit->GetNode(0)->data);
    if(strcmp(info->FSSignature, "KOTFS")){
        FileSystem::KFS* Fs = (FileSystem::KFS*)info->FSData;
        return Fs->remove((char*)filePathSplit->GetNode(1)->data);
    }
    return 0;
}