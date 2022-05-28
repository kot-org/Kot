#include "kfs.h"

namespace FileSystem{

    /* KFS */
    KFS::KFS(GPT::Partition* partition){
        globalPartition = partition;
        KFSPartitionInfo = (KFSinfo*)malloc(sizeof(KFSinfo));
        while(true){
            globalPartition->Read(0, sizeof(KFSinfo), KFSPartitionInfo);
            if(KFSPartitionInfo->IsInit.Data1 == GUIDData1 && KFSPartitionInfo->IsInit.Data2 == GUIDData2 && KFSPartitionInfo->IsInit.Data3 == GUIDData3 && KFSPartitionInfo->IsInit.Data4 == GUIDData4) break;  
            InitKFS();
        }     
    }

    void KFS::InitKFS(){
        globalLogs->Warning("Initialize KFS partition");
        uint64_t ClusterSize = PAGE_SIZE0;
        uintptr_t Cluster = malloc(ClusterSize);
        KFSinfo* info = (KFSinfo*)malloc(sizeof(KFSinfo));
        uint64_t MemTotPartiton = (globalPartition->partition->LastLBA - globalPartition->partition->FirstLBA) * globalPartition->port->GetSectorSizeLBA();

        info->IsInit.Data1 = GUIDData1;
        info->IsInit.Data2 = GUIDData2;
        info->IsInit.Data3 = GUIDData3;
        info->IsInit.Data4 = GUIDData4;

        info->NumberOfCluster = MemTotPartiton / ClusterSize;        
        info->BitmapSizeByte = DivideRoundUp(info->NumberOfCluster, 8);
        info->ClusterSize = ClusterSize;
        info->BitmapSizeCluster = DivideRoundUp(info->BitmapSizeByte, info->ClusterSize);
        info->BitmapPosition = ClusterSize;
        info->IndexToAllocate = 0;
        info->Root.FirstClusterForFile = info->BitmapPosition / ClusterSize + info->BitmapSizeCluster;
        info->Root.FirstClusterFile = 0;
        info->Root.FID = 0;
        char* Name = "Kot file system";

        int counter = strlen(Name);
        if(counter > MaxName) counter = MaxName;
        memcpy(info->Root.Name, Name, counter);

        globalPartition->Write(0, sizeof(KFSinfo), info);

        /* Clear Bitmap */        
        memset(Cluster, 0, info->ClusterSize);
        
        for(int i = 0; i < info->BitmapSizeCluster; i++){
            globalPartition->Write(info->BitmapPosition + (i * info->ClusterSize), info->ClusterSize, Cluster);
        }

        /* reload infos */
        globalPartition->Read(0, sizeof(KFSinfo), KFSPartitionInfo);

        /* Lock KFSInfo */
        for(int i = 0; i < info->Root.FirstClusterForFile; i++){
            LockCluster(i);
        }

        free(Cluster);        
        free((uintptr_t)info);
        
    }   


    AllocatePartition* KFS::Allocate(size_t size, Folder* folder, uint64_t LastClusterRequested, bool GetAutoLastCluster){
        globalLogs->Successful("");
        uint64_t NumberClusterToAllocate = DivideRoundUp(size, KFSPartitionInfo->ClusterSize);
        uint64_t ClusterAllocate = 1;
        uint64_t FirstBlocAllocated = 0;
        uint64_t NextCluster = 0;
        if(LastClusterRequested == 0 && GetAutoLastCluster){
            LastClusterRequested = KFSPartitionInfo->Root.LastClusterAllocated;
        }        
        if(folder != NULL) LastClusterRequested = folder->folderInfo->LastClusterRequested;
        uintptr_t Cluster = malloc(KFSPartitionInfo->ClusterSize);
        uintptr_t ClusterLast = malloc(KFSPartitionInfo->ClusterSize);

        for(int i = 0; i < NumberClusterToAllocate; i++){
            uint64_t ClusterRequested = RequestCluster();
            if(ClusterRequested != 0){
                ClusterAllocate++;

                if(FirstBlocAllocated == 0){ //for the return value
                    FirstBlocAllocated = ClusterRequested;
                }

                GetClusterData(ClusterRequested, Cluster);
                ClusterHeader* ClusterHeaderMain = (ClusterHeader*)Cluster;
                ClusterHeaderMain->LastCluster = LastClusterRequested;
                ClusterHeaderMain->NextCluster = 0;
                if(LastClusterRequested != 0){
                    GetClusterData(LastClusterRequested, ClusterLast);
                    ClusterHeader* ClusterHeaderLast = (ClusterHeader*)ClusterLast;
                    ClusterHeaderLast->NextCluster = ClusterRequested;
                    memcpy(ClusterLast, ClusterHeaderLast, sizeof(ClusterHeader));
                    SetClusterData(LastClusterRequested, ClusterLast);
                }
                
                memcpy(Cluster, ClusterHeaderMain, sizeof(ClusterHeader));
                SetClusterData(ClusterRequested, Cluster);
                LastClusterRequested = ClusterRequested;                 
            }else{
                break;
            }
        }
        if(ClusterAllocate < NumberClusterToAllocate && ClusterAllocate > 0){
            Free(FirstBlocAllocated);
            free(Cluster);
            free(ClusterLast);
            return NULL;
        }

        /* Update lastCluster requested */
        if(folder == NULL){
            KFSPartitionInfo->Root.LastClusterAllocated = FirstBlocAllocated;
            UpdatePartitionInfo();
        }else{
            folder->folderInfo->LastClusterRequested = FirstBlocAllocated;
            if(folder->folderInfo->FirstClusterData == 0){
                folder->folderInfo->FirstClusterData = FirstBlocAllocated;
            }
            UpdateFolderInfo(folder->folderInfo);
        }


        free(Cluster);
        free(ClusterLast);

        AllocatePartition* allocatePartition = (AllocatePartition*)malloc(sizeof(AllocatePartition));
        allocatePartition->FirstCluster = FirstBlocAllocated; 
        allocatePartition->LastCluster = LastClusterRequested;
        return allocatePartition;
    }

    void KFS::Free(uint64_t Cluster){
        uint64_t NextBlocToDelete = Cluster;
        uintptr_t ClusterData = calloc(KFSPartitionInfo->ClusterSize);
        while(true){
            GetClusterData(NextBlocToDelete, ClusterData);                
            uint64_t temp = NextBlocToDelete;
            NextBlocToDelete = ((ClusterHeader*)ClusterData)->NextCluster;
            UnlockCluster(temp);           

            if(NextBlocToDelete == 0){
                break;
            }
        }
      
        free(ClusterData);
    }

    uint64_t KFS::RequestCluster() {
        for (int i = KFSPartitionInfo->IndexToAllocate; i < KFSPartitionInfo->NumberOfCluster; i++) {
            if (CheckCluster(i)) { /* is free Cluster */
                LockCluster(i);
                return i;
            }             
        }
        return 0;
    }

    void KFS::LockCluster(uint64_t Cluster){
        uintptr_t BitmapBuffer = malloc(1); 
        globalPartition->Read(KFSPartitionInfo->BitmapPosition + (Cluster / 8), 1, BitmapBuffer);
        uint8_t value = *(uint8_t*)((uint64_t)BitmapBuffer);
        value = WriteBit(value, Cluster % 8, 1);
        *(uint8_t*)((uint64_t)BitmapBuffer) = value;
        globalPartition->Write(KFSPartitionInfo->BitmapPosition + (Cluster / 8), 1, BitmapBuffer);
        free(BitmapBuffer);
        KFSPartitionInfo->IndexToAllocate = Cluster;
        KFSPartitionInfo->NumberOfClusterUsed += 1;
        UpdatePartitionInfo();
    }

    void KFS::UnlockCluster(uint64_t Cluster){
        uintptr_t BitmapBuffer = malloc(1); 
        globalPartition->Read(KFSPartitionInfo->BitmapPosition + (Cluster / 8), 1, BitmapBuffer);
        uint8_t value = *(uint8_t*)((uint64_t)BitmapBuffer);
        value = WriteBit(value, Cluster % 8, 0);
        *(uint8_t*)((uint64_t)BitmapBuffer) = value;
        globalPartition->Write(KFSPartitionInfo->BitmapPosition + (Cluster / 8), 1, BitmapBuffer);
        free(BitmapBuffer);
        if(Cluster < KFSPartitionInfo->IndexToAllocate){
            KFSPartitionInfo->IndexToAllocate = Cluster;
        }
        KFSPartitionInfo->NumberOfClusterUsed -= 1;
        UpdatePartitionInfo();            
    }

    bool KFS::CheckCluster(uint64_t Cluster){
        bool Check = false;
        uintptr_t BitmapBuffer = malloc(1); 
        globalPartition->Read(KFSPartitionInfo->BitmapPosition + (Cluster / 8), 1, BitmapBuffer);
        uint8_t value = *(uint8_t*)((uint64_t)BitmapBuffer);
        
        free(BitmapBuffer); 
        Check = !ReadBit(value, Cluster % 8);
        return Check;
    }

    void KFS::GetClusterData(uint64_t Cluster, uintptr_t buffer){
        globalPartition->Read(Cluster * KFSPartitionInfo->ClusterSize, KFSPartitionInfo->ClusterSize, buffer);
    }

    void KFS::SetClusterData(uint64_t Cluster, uintptr_t buffer){
        globalPartition->Write(Cluster * KFSPartitionInfo->ClusterSize, KFSPartitionInfo->ClusterSize, buffer);
    }

    void KFS::flist(char* filePath){
        Node* FoldersSlit = split(filePath, "/");
        uint64_t count = FoldersSlit->GetSize();        

        if(KFSPartitionInfo->Root.FirstClusterFile == 0){
            globalLogs->Warning("The disk is empty");
            return;
        }

        uintptr_t Cluster = malloc(KFSPartitionInfo->ClusterSize);
        uint64_t ScanCluster = KFSPartitionInfo->Root.FirstClusterFile;
        ClusterHeader* ScanClusterHeader;
        HeaderInfo* ScanHeader;

        if(strlen(filePath) != 0){
            //search folder
            for(int i = 0; i <= count; i++){
                while(true){
                    GetClusterData(ScanCluster, Cluster);
                    ScanClusterHeader = (ClusterHeader*)Cluster;
                    ScanHeader = (HeaderInfo*)((uint64_t)Cluster + HeaderInfoPosition);
                
                    if(!ScanHeader->IsFile && ScanHeader->FID != 0){
                        FolderInfo* folderInfo = (FolderInfo*)((uint64_t)Cluster + FileInfoPosition);
                        if(strcmp(folderInfo->Name, (char*)FoldersSlit->GetNode(i)->data)){
                            ScanCluster = folderInfo->FirstClusterData;
                            if(ScanCluster == 0){
                                globalLogs->Warning("The folder is empty");
                                return;
                            }
                            break;
                        }
                    }
                    
                    if(ScanClusterHeader->NextCluster == NULL) return;
                    ScanCluster = ScanClusterHeader->NextCluster;
                }
            }
        }
        
        globalLogs->Message("Listing cluster : %u", ScanCluster);
        while(true){
            GetClusterData(ScanCluster, Cluster);
            ScanClusterHeader = (ClusterHeader*)Cluster;
            ScanHeader = (HeaderInfo*)((uint64_t)Cluster + HeaderInfoPosition);
         
            if(ScanHeader->IsFile){
                FileInfo* fileInfo = (FileInfo*)((uint64_t)Cluster + FileInfoPosition);
                globalLogs->Message("File Name : %s | FID : %u | Cluster : %u | Size : %u\n", fileInfo->Name, ScanHeader->FID, ScanCluster, fileInfo->BytesSize); 
            }else if(ScanHeader->FID != 0){
                FolderInfo* folderInfo = (FolderInfo*)((uint64_t)Cluster + FileInfoPosition);
                globalLogs->Message("Folder Name : %s | FID : %u | Cluster : %u", folderInfo->Name, ScanHeader->FID, ScanCluster);
            }
            
            if(ScanClusterHeader->NextCluster == NULL) break;
            ScanCluster = ScanClusterHeader->NextCluster;
        }
        free((uintptr_t)Cluster);
    }

    bool KFS::IsDirExist(char* filePath){
        Node* FoldersSlit = split(filePath, "/");
        uint64_t count = FoldersSlit->GetSize(); 
        count--;   

        if(KFSPartitionInfo->Root.FirstClusterFile == 0){
            globalLogs->Warning("The disk is empty");
            return false;
        }

        uintptr_t Cluster = malloc(KFSPartitionInfo->ClusterSize);
        uint64_t ScanCluster = KFSPartitionInfo->Root.FirstClusterFile;
        ClusterHeader* ScanClusterHeader;
        HeaderInfo* ScanHeader;

        if(strlen(filePath) != 0){
            //search folder
            for(int i = 0; i <= count; i++){
                while(true){
                    GetClusterData(ScanCluster, Cluster);
                    ScanClusterHeader = (ClusterHeader*)Cluster;
                    ScanHeader = (HeaderInfo*)((uint64_t)Cluster + HeaderInfoPosition);
                
                    if(!ScanHeader->IsFile && ScanHeader->FID != 0){
                        FolderInfo* folderInfo = (FolderInfo*)((uint64_t)Cluster + FileInfoPosition);
                        if(strcmp(folderInfo->Name, (char*)FoldersSlit->GetNode(i)->data)){
                            ScanCluster = folderInfo->FirstClusterData;
                            break;
                        }
                    }
                    
                    if(ScanClusterHeader->NextCluster == NULL){
                        free((uintptr_t)Cluster);
                        return false;
                    } 
                    ScanCluster = ScanClusterHeader->NextCluster;
                }
            }
        }

        free((uintptr_t)Cluster);
        return true;
    }

    uint64_t KFS::mkdir(char* filePath, uint64_t Mode){
        if(IsDirExist(filePath)) return 0;
        Node* FoldersSlit = split(filePath, "/");
        uint64_t count = FoldersSlit->GetSize();  

        uintptr_t Cluster = malloc(KFSPartitionInfo->ClusterSize);
        uint64_t ScanCluster = KFSPartitionInfo->Root.FirstClusterFile;
        ClusterHeader* ScanClusterHeader;
        HeaderInfo* ScanHeader;
        Folder* folder = NULL;


        for(int i = 0; i < count - 1; i++){
            while(true){
                GetClusterData(ScanCluster, Cluster);
                ScanClusterHeader = (ClusterHeader*)Cluster;
                ScanHeader = (HeaderInfo*)((uint64_t)Cluster + HeaderInfoPosition);
             
                if(!ScanHeader->IsFile && ScanHeader->FID != 0){
                    FolderInfo* folderInfo = (FolderInfo*)((uint64_t)Cluster + FileInfoPosition);
                    if(strcmp(folderInfo->Name, (char*)FoldersSlit->GetNode(i)->data)){
                        ScanCluster = folderInfo->FirstClusterData;
                        if(folder != NULL){
                            free(folder->folderInfo);
                            free(folder);
                        }

                        folder = (Folder*)malloc(sizeof(Folder));
                        folder->folderInfo = (FolderInfo*)malloc(sizeof(FolderInfo));
                        memcpy(folder->folderInfo, folderInfo, sizeof(FolderInfo));
                        folder->kfs = this;
                        break;
                    }
                }
                
                if(ScanClusterHeader->NextCluster == NULL) return 2;
                ScanCluster = ScanClusterHeader->NextCluster;
            }

            if(i == (count - 2)) break; 
        }

        uint64_t ClusterSize = DivideRoundUp(DataPosition, KFSPartitionInfo->ClusterSize);
        uint64_t ClusterSizeFolder = KFSPartitionInfo->ClusterSize * ClusterSize;
        AllocatePartition* allocatePartition = Allocate(ClusterSizeFolder, folder, 0, true);
        uint64_t ParentClusterHeaderPostition = 0;
        if(folder != NULL){
            ParentClusterHeaderPostition = folder->folderInfo->ClusterHeaderPostition;
            free(folder->folderInfo);
            free(folder);
        }
        folder = (Folder*)malloc(sizeof(Folder)); 
        folder->folderInfo = (FolderInfo*)malloc(sizeof(FolderInfo));
        uint64_t ClusterPosition = allocatePartition->FirstCluster; 
        free(allocatePartition);
        if(KFSPartitionInfo->Root.FirstClusterFile == 0){
            KFSPartitionInfo->Root.FirstClusterFile = ClusterPosition;
            UpdatePartitionInfo();
        }
        GetClusterData(ClusterPosition, Cluster);
        HeaderInfo* Header = (HeaderInfo*)(uintptr_t)((uint64_t)Cluster + sizeof(ClusterHeader));
        Header->FID = GetFID();
        Header->IsFile = false;
        Header->ParentLocationCluster = ParentClusterHeaderPostition;
        FolderInfo* folderInfo = (FolderInfo*)(uintptr_t)((uint64_t)Cluster + sizeof(ClusterHeader) + sizeof(HeaderInfo));
        folderInfo->FirstClusterData = 0;
        folderInfo->BytesSize = 0;
        folderInfo->ClusterHeaderPostition = ClusterPosition;

        int counter = strlen(filePath);
        if(counter > MaxPath) counter = MaxPath;
        memcpy(folderInfo->Path, filePath, counter);
        folderInfo->Path[counter] = 0;

        counter = strlen(filePath);
        if(counter > MaxName) counter = MaxName;
        memcpy(folderInfo->Name, FoldersSlit->GetNode(count - 1)->data, counter);
        folderInfo->Name[counter] = 0;
        RealTimeClock* realTimeClock;
        folderInfo->TimeInfoFS.CreateTime.seconds = realTimeClock->readSeconds();
        folderInfo->TimeInfoFS.CreateTime.minutes = realTimeClock->readMinutes();
        folderInfo->TimeInfoFS.CreateTime.hours = realTimeClock->readHours();
        folderInfo->TimeInfoFS.CreateTime.days = realTimeClock->readDay();
        folderInfo->TimeInfoFS.CreateTime.months = realTimeClock->readMonth();
        folderInfo->TimeInfoFS.CreateTime.years = realTimeClock->readYear() + 2000;

        folderInfo->Mode = Mode;

        UpdateFolderSize(folderInfo, ClusterSize);

        memcpy((uintptr_t)((uint64_t)Cluster + HeaderInfoPosition), Header, sizeof(HeaderInfo));
        memcpy((uintptr_t)((uint64_t)Cluster + FileInfoPosition), folderInfo, sizeof(FolderInfo));
        
        SetClusterData(ClusterPosition, Cluster);
        
        free(Cluster);
        
        return 1; //sucess
    }

    bool KFS::readdir(char* filePath, Folder* folder){
        Node* FoldersSlit = split(filePath, "/");
        uint64_t count = FoldersSlit->GetSize();   
        count--;

        if(KFSPartitionInfo->Root.FirstClusterFile == 0){
            globalLogs->Warning("The disk is empty");
            return false;
        }

        uintptr_t Cluster = malloc(KFSPartitionInfo->ClusterSize);
        uint64_t ScanCluster = KFSPartitionInfo->Root.FirstClusterFile;
        ClusterHeader* ScanClusterHeader;
        HeaderInfo* ScanHeader;

        for(int i = 0; i <= count; i++){
            while(true){
                GetClusterData(ScanCluster, Cluster);
                ScanClusterHeader = (ClusterHeader*)Cluster;
                ScanHeader = (HeaderInfo*)((uint64_t)Cluster + HeaderInfoPosition);
             
                if(!ScanHeader->IsFile && ScanHeader->FID != 0){
                    FolderInfo* folderInfo = (FolderInfo*)((uint64_t)Cluster + FileInfoPosition);
                    if(strcmp(folderInfo->Name, (char*)FoldersSlit->GetNode(i)->data)){
                        ScanCluster = folderInfo->FirstClusterData;
                        if(i == count){
                            folder->folderInfo = (FolderInfo*)malloc(sizeof(FolderInfo));
                            folder->kfs = this;
                            memcpy(folder->folderInfo, folderInfo, sizeof(FolderInfo));
                            free((uintptr_t)Cluster);
                            return true;
                        }

                        if(ScanCluster == 0){
                            free((uintptr_t)Cluster);
                            return false;
                        }

                        break;
                    }
                }

                ScanCluster = ScanClusterHeader->NextCluster;
                if(ScanCluster == 0){
                    free((uintptr_t)Cluster);
                    return false;
                }
            }
        }
        free((uintptr_t)Cluster);

        if(folder == NULL){
            return false;
        }else{
            return true;
        }
    }

    Folder* KFS::readdirWithCluster(uint64_t cluster){
        uintptr_t Cluster = malloc(KFSPartitionInfo->ClusterSize);
        GetClusterData(cluster, Cluster);
        
        FolderInfo* folderInfo = (FolderInfo*)((uint64_t)Cluster + FileInfoPosition);
        Folder* folder = NULL;
        folder = (Folder*)malloc(sizeof(Folder));
        folder->folderInfo = (FolderInfo*)malloc(sizeof(FolderInfo));
        folder->kfs = this;
        memcpy(folder->folderInfo, folderInfo, sizeof(FolderInfo));
        
        free((uintptr_t)Cluster);
        return folder;
    }

    File* KFS::fopenWithCluster(uint64_t cluster){
        uintptr_t Cluster = malloc(KFSPartitionInfo->ClusterSize);
        GetClusterData(cluster, Cluster);
        
        FileInfo* fileInfo = (FileInfo*)((uint64_t)Cluster + FileInfoPosition);
        File* file = (File*)malloc(sizeof(File));
        file->kfs = this;
        memcpy(&file->fileInfo, fileInfo, sizeof(FileInfo));
        
        free((uintptr_t)Cluster);
        return file;
    }

    bool KFS::fopen(char* filePath, char* Mode, File* returnData){
        Node* FoldersSlit = split(filePath, "/");
        uint64_t count = FoldersSlit->GetSize();   
        count--;
        uintptr_t Cluster = malloc(KFSPartitionInfo->ClusterSize);
        char* FileName;
        char* FileNameSearch = (char*)FoldersSlit->GetNode(count)->data;
        uint64_t ScanCluster = KFSPartitionInfo->Root.FirstClusterFile;
        ClusterHeader* ScanClusterHeader;
        HeaderInfo* ScanHeader;

        Folder* folder = NULL;

        if(KFSPartitionInfo->Root.FirstClusterFile == 0 && count == 0){
            free((uintptr_t)Cluster);
            memcpy(&returnData->fileInfo, NewFile(filePath, folder), sizeof(FileInfo));
            returnData->Mode = Mode;
            returnData->kfs = this;
            return true;
        }else if(KFSPartitionInfo->Root.FirstClusterFile == 0 && count != 0){
            free((uintptr_t)Cluster);
            return false;
        }

        for(int i = 0; i <= count; i++){
            while(true){
                GetClusterData(ScanCluster, Cluster);
                ScanClusterHeader = (ClusterHeader*)Cluster;
                ScanHeader = (HeaderInfo*)((uint64_t)Cluster + sizeof(ClusterHeader));
             
                if(ScanHeader->IsFile){
                    FileInfo* fileInfo = (FileInfo*)((uint64_t)Cluster + sizeof(ClusterHeader) + sizeof(HeaderInfo));
                    FileName = fileInfo->Name;
                    if(i == count && strcmp(FileName, (char*)FileNameSearch)){
                        memcpy(&returnData->fileInfo, fileInfo, sizeof(FileInfo));
                        returnData->Mode = Mode;
                        returnData->kfs = this;
                        free((uintptr_t)Cluster);
                        if(folder != NULL){
                            free((uintptr_t)folder);
                        }
                        return true;
                    }
                }else if(ScanHeader->FID != 0){
                    FolderInfo* folderInfo = (FolderInfo*)((uint64_t)Cluster + sizeof(ClusterHeader) + sizeof(HeaderInfo));
                    FileName = folderInfo->Name;                  
                    if(i == count){
                        free((uintptr_t)Cluster);
                        return false;
                    }

                    if(strcmp(FileName, (char*)FoldersSlit->GetNode(i)->data)){
                        ScanCluster = folderInfo->FirstClusterData;
                        if(folder != NULL){
                            free((uintptr_t)folder->folderInfo);
                            free((uintptr_t)folder);
                        }
                        folder = (Folder*)malloc(sizeof(Folder));
                        folder->folderInfo = (FolderInfo*)malloc(sizeof(FolderInfo));
                        memcpy(folder->folderInfo, folderInfo, sizeof(FolderInfo));
                        folder->kfs = this;
                        break;
                    }
                }

                ScanCluster = ScanClusterHeader->NextCluster;
                if(ScanCluster == 0){
                    if(i == count){
                        free((uintptr_t)Cluster);
                        memcpy(&returnData->fileInfo, NewFile(filePath, folder), sizeof(FileInfo));
                        returnData->Mode = Mode;
                        returnData->kfs = this;
                        return true;
                    }
                }
            }            
        } 

        if(folder != NULL){
            free((uintptr_t)folder);
        }
        free((uintptr_t)Cluster);
        return false;
    }

    FileInfo* KFS::NewFile(char* filePath, Folder* folder){
        uint64_t ClusterSize = DivideRoundUp(DataPosition, KFSPartitionInfo->ClusterSize);
        uint64_t FileClusterSize = KFSPartitionInfo->ClusterSize * ClusterSize; //alloc one bloc, for the header and data
        AllocatePartition* allocatePartition = Allocate(FileClusterSize, folder, 0, true);
        uint64_t ClusterLastAllocate = allocatePartition->LastCluster;
        uint64_t ClusterPosition = allocatePartition->FirstCluster; 
        free(allocatePartition);
        if(KFSPartitionInfo->Root.FirstClusterFile == 0){
            KFSPartitionInfo->Root.FirstClusterFile = ClusterPosition;
        }
        
        uintptr_t Cluster = calloc(KFSPartitionInfo->ClusterSize);
        GetClusterData(ClusterPosition, Cluster);
        HeaderInfo* Header = (HeaderInfo*)(uintptr_t)((uint64_t)Cluster + sizeof(ClusterHeader));
        Header->FID = GetFID();
        Header->IsFile = true;
        if(folder != NULL){
            Header->ParentLocationCluster = folder->folderInfo->ClusterHeaderPostition;
        }else{
            Header->ParentLocationCluster = 0;
        }

        FileInfo* fileInfo = (FileInfo*)(uintptr_t)((uint64_t)Cluster + sizeof(ClusterHeader) + sizeof(HeaderInfo));
        fileInfo->LastClusterOfTheFile = ClusterLastAllocate;
        fileInfo->ClusterHeaderPostition = ClusterPosition;
        fileInfo->NextCluster = 0;

        int counter = strlen(filePath);
        if(counter > MaxPath) counter = MaxPath;
        memcpy(fileInfo->Path, filePath, counter);
        fileInfo->Path[counter] = 0;

        Node* FoldersSlit = split(filePath, "/");
        uint64_t count = FoldersSlit->GetSize();   
        counter = strlen((char*)FoldersSlit->GetNode(count - 1)->data);
        if(counter > MaxName) counter = MaxName;
        memcpy(fileInfo->Name, FoldersSlit->GetNode(count - 1)->data, counter);
        fileInfo->Name[counter] = 0;
        

        RealTimeClock* realTimeClock;
        fileInfo->TimeInfoFS.CreateTime.seconds = realTimeClock->readSeconds();
        fileInfo->TimeInfoFS.CreateTime.minutes = realTimeClock->readMinutes();
        fileInfo->TimeInfoFS.CreateTime.hours = realTimeClock->readHours();
        fileInfo->TimeInfoFS.CreateTime.days = realTimeClock->readDay();
        fileInfo->TimeInfoFS.CreateTime.months = realTimeClock->readMonth();
        fileInfo->TimeInfoFS.CreateTime.years = realTimeClock->readYear() + 2000;

        memcpy((uintptr_t)((uint64_t)Cluster + HeaderInfoPosition), Header, sizeof(HeaderInfo));
        memcpy((uintptr_t)((uint64_t)Cluster + FileInfoPosition), fileInfo, sizeof(FileInfo));
        SetClusterData(ClusterPosition, Cluster);
        fileInfo = (FileInfo*)malloc(sizeof(FileInfo));
        memcpy(fileInfo, (uintptr_t)((uint64_t)Cluster + FileInfoPosition), sizeof(FileInfo));
        free(Cluster);

        UpdateFileSize(fileInfo, 0, ClusterSize);

        return fileInfo;
    }

    uint64_t KFS::GetFID(){
        KFSPartitionInfo->Root.FID++;
        while(!UpdatePartitionInfo());
        return KFSPartitionInfo->Root.FID;
    }

    uint64_t KFS::rename(char* oldPath, char* newPath){
        if(KFSPartitionInfo->Root.FirstClusterFile == 0){
            globalLogs->Warning("The disk is empty");
            return 0;
        }

        if(strlen(oldPath) == 0 || newPath == 0) return 2;

        uintptr_t Cluster = malloc(KFSPartitionInfo->ClusterSize);

        Node* FoldersSlitOld = split(oldPath, "/");
        int countOld = FoldersSlitOld->GetSize();

        Node* FoldersSlitNew = split(newPath, "/");
        int countNew = FoldersSlitNew->GetSize();
        countNew--;  

        //find file / folder
        uint64_t OldClusterLocation = KFSPartitionInfo->Root.FirstClusterFile;
        for(int i = 0; i < countOld; i++){
            while(true){
                GetClusterData(OldClusterLocation, Cluster);
                ClusterHeader* ScanClusterHeader = (ClusterHeader*)Cluster;
                HeaderInfo* ScanHeader = (HeaderInfo*)((uint64_t)Cluster + HeaderInfoPosition);

                if(!ScanHeader->IsFile && ScanHeader->FID != 0){
                    FolderInfo* folderInfo = (FolderInfo*)((uint64_t)Cluster + FileInfoPosition);
                    if(strcmp(folderInfo->Name, (char*)FoldersSlitOld->GetNode(i)->data)){
                        if(countOld - 1 != i){
                            OldClusterLocation = folderInfo->FirstClusterData;
                        }else{
                            OldClusterLocation = folderInfo->ClusterHeaderPostition;
                        }                        
                        break;
                    }
                }else if(ScanHeader->IsFile && ScanHeader->FID != 0){
                    FileInfo* fileInfo = (FileInfo*)((uint64_t)Cluster + FileInfoPosition);
                    if(strcmp(fileInfo->Name, (char*)FoldersSlitOld->GetNode(i)->data)){
                        OldClusterLocation = fileInfo->ClusterHeaderPostition;
                        break;
                    }
                }
                
                if(ScanClusterHeader->NextCluster == NULL) return 0;
                OldClusterLocation = ScanClusterHeader->NextCluster;
            }
        }

        //search new location
        uint64_t NewClusterLocation = KFSPartitionInfo->Root.FirstClusterFile;
        for(int i = 0; i < countNew; i++){
            while(true){
                GetClusterData(NewClusterLocation, Cluster);
                ClusterHeader* ScanClusterHeader = (ClusterHeader*)Cluster;
                HeaderInfo* ScanHeader = (HeaderInfo*)((uint64_t)Cluster + HeaderInfoPosition);
            
                if(!ScanHeader->IsFile && ScanHeader->FID != 0){
                    FolderInfo* folderInfo = (FolderInfo*)((uint64_t)Cluster + FileInfoPosition);
                    if(strcmp(folderInfo->Name, (char*)FoldersSlitNew->GetNode(i)->data)){
                        NewClusterLocation = folderInfo->FirstClusterData;
                        break;
                    }
                }
                
                if(ScanClusterHeader->NextCluster == NULL) return 0;
                NewClusterLocation = ScanClusterHeader->NextCluster;
            }
        }

        ClusterHeader* ScanClusterHeader = GetClusterHeader(OldClusterLocation);
        HeaderInfo* ScanHeader = GetHeaderInfo(OldClusterLocation);
        ClusterHeader* lastHeader = GetClusterHeader(ScanClusterHeader->LastCluster);
        ClusterHeader* nextHeader = GetClusterHeader(ScanClusterHeader->NextCluster);

        if(ScanHeader->ParentLocationCluster != NULL){          
            Folder* folderParent = readdirWithCluster(ScanHeader->ParentLocationCluster);

            if(OldClusterLocation == folderParent->folderInfo->FirstClusterData){
                if(folderParent->GetNumberOfFiles() != 0){
                    folderParent->folderInfo->FirstClusterData = ScanClusterHeader->NextCluster;
                }else{
                    folderParent->folderInfo->FirstClusterData = 0;
                }

            }else{
                lastHeader->NextCluster = ScanClusterHeader->NextCluster;
            }

            if(OldClusterLocation == folderParent->folderInfo->LastClusterRequested){
                folderParent->folderInfo->LastClusterRequested = ScanClusterHeader->LastCluster;
            }else{
                nextHeader->LastCluster = ScanClusterHeader->LastCluster;
            }

            UpdateFolderInfo(folderParent->folderInfo);
            free(folderParent->folderInfo);                              
            free(folderParent);                              
        }else{
            //the parent is root
            if(KFSPartitionInfo->Root.FirstClusterFile == OldClusterLocation){
                if(ScanClusterHeader->LastCluster != NULL){
                    KFSPartitionInfo->Root.FirstClusterFile = ScanClusterHeader->LastCluster;
                }else{
                    KFSPartitionInfo->Root.FirstClusterFile = ScanClusterHeader->NextCluster;
                }
                UpdatePartitionInfo();
            }

            if(KFSPartitionInfo->Root.LastClusterAllocated == OldClusterLocation){
                if(ScanClusterHeader->NextCluster != NULL){
                    KFSPartitionInfo->Root.LastClusterAllocated = ScanClusterHeader->NextCluster;
                }else{
                    KFSPartitionInfo->Root.LastClusterAllocated = ScanClusterHeader->LastCluster;
                }
                UpdatePartitionInfo();
            }

            nextHeader->LastCluster = ScanClusterHeader->LastCluster;
            lastHeader->NextCluster = ScanClusterHeader->NextCluster;
        }
        
        UpdateClusterHeader(lastHeader, ScanClusterHeader->LastCluster);
        UpdateClusterHeader(nextHeader, ScanClusterHeader->NextCluster);

        free(ScanClusterHeader);
        free(lastHeader);
        free(nextHeader);
        free(ScanHeader);

        //relocate
        ScanClusterHeader = GetClusterHeader(NewClusterLocation);
        ScanHeader = GetHeaderInfo(NewClusterLocation);
        
        if(ScanHeader->ParentLocationCluster != NULL){    
            Folder* folderParent = readdirWithCluster(ScanHeader->ParentLocationCluster);

            uint64_t LastClusterUsedByFile = folderParent->folderInfo->LastClusterRequested;
            lastHeader = GetClusterHeader(LastClusterUsedByFile);
            //the parent is root
            if(folderParent->folderInfo->FirstClusterData == 0){
                folderParent->folderInfo->FirstClusterData = OldClusterLocation;
                UpdatePartitionInfo();
            }
            folderParent->folderInfo->LastClusterRequested = OldClusterLocation;

            lastHeader->NextCluster = OldClusterLocation;
            UpdateClusterHeader(lastHeader, LastClusterUsedByFile);

            UpdateFolderInfo(folderParent->folderInfo);
            free(folderParent->folderInfo);                              
            free(folderParent);                              
        }else{  
            uint64_t LastClusterUsedByFile = KFSPartitionInfo->Root.LastClusterAllocated;
            lastHeader = GetClusterHeader(LastClusterUsedByFile);
            //the parent is root
            if(KFSPartitionInfo->Root.FirstClusterFile == 0){
                KFSPartitionInfo->Root.FirstClusterFile = OldClusterLocation;
                UpdatePartitionInfo();
            }

            KFSPartitionInfo->Root.LastClusterAllocated = OldClusterLocation;
            UpdatePartitionInfo();

            lastHeader->NextCluster = OldClusterLocation;
            UpdateClusterHeader(lastHeader, LastClusterUsedByFile);
        }

        HeaderInfo* ScanHeaderData = GetHeaderInfo(OldClusterLocation);
        if(ScanHeaderData->IsFile){

            File* file = fopenWithCluster(OldClusterLocation);

            int counter = strlen((char*)FoldersSlitNew->GetNode(countNew)->data);
            if(counter > MaxName) counter = MaxName;
            memcpy(file->fileInfo.Name, FoldersSlitNew->GetNode(countNew)->data, counter);
            file->fileInfo.Name[counter] = 0;

            UpdateFileInfo(&file->fileInfo);

            free(file);           
        }else{
            Folder* folder = readdirWithCluster(OldClusterLocation);

            int counter = strlen((char*)FoldersSlitNew->GetNode(countNew)->data);
            if(counter > MaxName) counter = MaxName;
            memcpy(folder->folderInfo->Name, FoldersSlitNew->GetNode(countNew)->data, counter);
            folder->folderInfo->Name[counter] = 0;
            UpdateFolderInfo(folder->folderInfo);

            free(folder->folderInfo);
            free(folder);
        }

        //update file path for all files or folder move

        return 1;
    }

    uint64_t KFS::remove(char* filePath){
        Node* FoldersSlit = split(filePath, "/");
        int count = FoldersSlit->GetSize();         

        if(KFSPartitionInfo->Root.FirstClusterFile == 0){
            globalLogs->Warning("The disk is empty");
            return 0;
        }

        uintptr_t Cluster = malloc(KFSPartitionInfo->ClusterSize);
        uint64_t ScanCluster = KFSPartitionInfo->Root.FirstClusterFile;
        ClusterHeader* ScanClusterHeader;
        HeaderInfo* ScanHeader;
        FolderInfo* folderInfoToDelete;

        if(strlen(filePath) != 0){
            //search folder
            for(int i = 0; i <= count; i++){
                while(true){
                    GetClusterData(ScanCluster, Cluster);
                    ScanClusterHeader = (ClusterHeader*)Cluster;
                    ScanHeader = (HeaderInfo*)((uint64_t)Cluster + HeaderInfoPosition);
                
                    if(ScanHeader->FID != 0){
                        if(ScanHeader->IsFile){
                            FileInfo* fileInfo = (FileInfo*)((uint64_t)Cluster + FileInfoPosition);
                            if(strcmp(fileInfo->Name, (char*)FoldersSlit->GetNode(i)->data)){
                                DeleteFile(fileInfo);
                            }
                        }else{
                            FolderInfo* folderInfo = (FolderInfo*)((uint64_t)Cluster + FileInfoPosition);
                            if(strcmp(folderInfo->Name, (char*)FoldersSlit->GetNode(i)->data)){
                                ScanCluster = folderInfo->FirstClusterData;
                                if(ScanCluster == 0){
                                    free(Cluster);
                                    //the folder is empty
                                    if(i == count){
                                        CleanFolder(folderInfo);
                                        return 1;
                                    }else{
                                        //folder doesn't exist
                                        return 0;
                                    }                            
                                }

                                folderInfoToDelete = folderInfo;
                                break;
                            }                            
                        }

                    }
                    
                    if(ScanClusterHeader->NextCluster == NULL) return 0;
                    ScanCluster = ScanClusterHeader->NextCluster;
                }
            }
        }else{
            while(true){
                GetClusterData(ScanCluster, Cluster);
                ScanClusterHeader = (ClusterHeader*)Cluster;
                ScanHeader = (HeaderInfo*)((uint64_t)Cluster + HeaderInfoPosition);
            
                if(ScanHeader->IsFile){
                    FileInfo* fileInfo = (FileInfo*)((uint64_t)Cluster + FileInfoPosition);
                    DeleteFile(fileInfo);
                }else if(ScanHeader->FID != 0){
                    FolderInfo* folderInfo = (FolderInfo*)((uint64_t)Cluster + FileInfoPosition);
                    CleanFolder(folderInfo);
                }
                
                if(ScanClusterHeader->NextCluster == NULL) break;
                ScanCluster = ScanClusterHeader->NextCluster;
            }
            free(Cluster);
            return 1;
        }
        
        CleanFolder(folderInfoToDelete);
        free(Cluster);
    }    

    void KFS::DeleteFile(FileInfo* fileInfo){
        UpdateFileSize(fileInfo, 0, 0);
        UpdateFileInfo(fileInfo);
        ClusterHeader* ScanClusterHeader = GetClusterHeader(fileInfo->ClusterHeaderPostition);
        HeaderInfo* ScanHeader = GetHeaderInfo(fileInfo->ClusterHeaderPostition);
        ClusterHeader* lastHeader = GetClusterHeader(ScanClusterHeader->LastCluster);
        ClusterHeader* nextHeader = GetClusterHeader(ScanClusterHeader->NextCluster);

        if(fileInfo->NextCluster != NULL){
            Free(fileInfo->NextCluster);
        }

        if(ScanHeader->ParentLocationCluster != NULL){            
            Folder* folderParent = readdirWithCluster(ScanHeader->ParentLocationCluster);

            if(fileInfo->ClusterHeaderPostition == folderParent->folderInfo->FirstClusterData){
                if(folderParent->GetNumberOfFiles() != 0){
                    folderParent->folderInfo->FirstClusterData = ScanClusterHeader->NextCluster;
                }else{
                    folderParent->folderInfo->FirstClusterData = 0;
                }

            }else{
                lastHeader->NextCluster = ScanClusterHeader->NextCluster;
            }

            if(fileInfo->ClusterHeaderPostition == folderParent->folderInfo->LastClusterRequested){
                folderParent->folderInfo->LastClusterRequested = ScanClusterHeader->LastCluster;
            }else{
                nextHeader->LastCluster = ScanClusterHeader->LastCluster;
            }

            UpdateFolderInfo(folderParent->folderInfo);
            free(folderParent->folderInfo);                              
            free(folderParent);                              
        }else{
            //the parent is root
            if(KFSPartitionInfo->Root.FirstClusterFile == fileInfo->ClusterHeaderPostition){
                KFSPartitionInfo->Root.FirstClusterFile = ScanClusterHeader->NextCluster;
                UpdatePartitionInfo();
            }

            if(KFSPartitionInfo->Root.LastClusterAllocated == fileInfo->ClusterHeaderPostition){
                KFSPartitionInfo->Root.LastClusterAllocated = ScanClusterHeader->LastCluster;
                UpdatePartitionInfo();
            }

            nextHeader->LastCluster = ScanClusterHeader->LastCluster;
            lastHeader->NextCluster = ScanClusterHeader->NextCluster;
        }
        
        UpdateClusterHeader(lastHeader, ScanClusterHeader->LastCluster);
        UpdateClusterHeader(nextHeader, ScanClusterHeader->NextCluster);

        UnlockCluster(fileInfo->ClusterHeaderPostition);

        free(ScanClusterHeader);
        free(lastHeader);
        free(nextHeader);
        free(ScanHeader);
    }

    void KFS::CleanFolder(FolderInfo* folderInfo){
        uintptr_t Cluster = calloc(KFSPartitionInfo->ClusterSize);
        uint64_t ScanCluster = folderInfo->FirstClusterData;
        ClusterHeader* ScanClusterHeader;
        HeaderInfo* ScanHeader;
        while(true){
            GetClusterData(ScanCluster, Cluster);
            ScanClusterHeader = (ClusterHeader*)Cluster;
            ScanHeader = (HeaderInfo*)((uint64_t)Cluster + HeaderInfoPosition);
         
            if(ScanHeader->IsFile){
                FileInfo* fileInfo = (FileInfo*)((uint64_t)Cluster + FileInfoPosition);
                DeleteFile(fileInfo);

            }else if(ScanHeader->FID != 0){
                Folder* folderChildren = readdirWithCluster(ScanCluster);
                FolderInfo* folderInfoChildren = folderChildren->folderInfo;
                ScanClusterHeader = GetClusterHeader(folderInfoChildren->ClusterHeaderPostition);

                CleanFolder(folderInfoChildren);

                UpdateFolderSize(folderInfoChildren, 0);
                UpdateFolderInfo(folderInfoChildren);

                ClusterHeader* lastHeader = GetClusterHeader(ScanClusterHeader->LastCluster);
                ClusterHeader* nextHeader = GetClusterHeader(ScanClusterHeader->NextCluster);

                if(ScanHeader->ParentLocationCluster != NULL){            
                    Folder* folderParent = readdirWithCluster(ScanHeader->ParentLocationCluster);

                    if(folderInfoChildren->ClusterHeaderPostition == folderParent->folderInfo->FirstClusterData){
                        if(folderParent->GetNumberOfFiles() != 0){
                            folderParent->folderInfo->FirstClusterData = ScanClusterHeader->NextCluster;
                        }else{
                            folderParent->folderInfo->FirstClusterData = 0;
                        }

                    }else{
                        lastHeader->NextCluster = ScanClusterHeader->NextCluster;
                    }

                    if(folderInfoChildren->ClusterHeaderPostition == folderParent->folderInfo->LastClusterRequested){
                        folderParent->folderInfo->LastClusterRequested = ScanClusterHeader->LastCluster;
                    }else{
                        nextHeader->LastCluster = ScanClusterHeader->LastCluster;
                    }

                    UpdateFolderInfo(folderParent->folderInfo);
                    free(folderParent->folderInfo);                              
                    free(folderParent);                              
                }else{
                    //the parent is root
                    if(KFSPartitionInfo->Root.FirstClusterFile == folderInfo->ClusterHeaderPostition){
                        KFSPartitionInfo->Root.FirstClusterFile = ScanClusterHeader->NextCluster;
                        UpdatePartitionInfo();
                    }

                    if(KFSPartitionInfo->Root.LastClusterAllocated == folderInfo->ClusterHeaderPostition){
                        KFSPartitionInfo->Root.LastClusterAllocated = ScanClusterHeader->LastCluster;
                        UpdatePartitionInfo();
                    }

                    nextHeader->LastCluster = ScanClusterHeader->LastCluster;
                    lastHeader->NextCluster = ScanClusterHeader->NextCluster;
                }
                
                UpdateClusterHeader(lastHeader, ScanClusterHeader->LastCluster);
                UpdateClusterHeader(nextHeader, ScanClusterHeader->NextCluster);

                UnlockCluster(folderInfoChildren->ClusterHeaderPostition);

                free(folderInfoChildren);
                free(folderChildren);
                free(ScanClusterHeader);
                free(lastHeader);
                free(nextHeader);
            }
            
            if(ScanClusterHeader->NextCluster == NULL) break;
            ScanCluster = ScanClusterHeader->NextCluster;
        }

        //Delete this folder 
       
        UpdateFolderSize(folderInfo, 0);
        UpdateFolderInfo(folderInfo);
        ScanClusterHeader = GetClusterHeader(folderInfo->ClusterHeaderPostition);
        ScanHeader = GetHeaderInfo(folderInfo->ClusterHeaderPostition);

        ClusterHeader* lastHeader = GetClusterHeader(ScanClusterHeader->LastCluster);
        ClusterHeader* nextHeader = GetClusterHeader(ScanClusterHeader->NextCluster);

        if(ScanHeader->ParentLocationCluster != NULL){         
            Folder* folderParent = readdirWithCluster(ScanHeader->ParentLocationCluster);

            if(folderInfo->ClusterHeaderPostition == folderParent->folderInfo->FirstClusterData){
                if(folderParent->GetNumberOfFiles() != 0){
                    folderParent->folderInfo->FirstClusterData = ScanClusterHeader->NextCluster;
                }else{
                    folderParent->folderInfo->FirstClusterData = 0;
                }

            }else{
                lastHeader->NextCluster = ScanClusterHeader->NextCluster;
            }

            if(folderInfo->ClusterHeaderPostition == folderParent->folderInfo->LastClusterRequested){
                folderParent->folderInfo->LastClusterRequested = ScanClusterHeader->LastCluster;
            }else{
                nextHeader->LastCluster = ScanClusterHeader->LastCluster;
            }

            UpdateFolderInfo(folderParent->folderInfo);
            free(folderParent->folderInfo);                              
            free(folderParent);                              
        }else{
            //the parent is root
            if(KFSPartitionInfo->Root.FirstClusterFile == folderInfo->ClusterHeaderPostition){
                KFSPartitionInfo->Root.FirstClusterFile = ScanClusterHeader->NextCluster;
                UpdatePartitionInfo();
            }

            if(KFSPartitionInfo->Root.LastClusterAllocated == folderInfo->ClusterHeaderPostition){
                KFSPartitionInfo->Root.LastClusterAllocated = ScanClusterHeader->LastCluster;
                UpdatePartitionInfo();
            }

            nextHeader->LastCluster = ScanClusterHeader->LastCluster;
            lastHeader->NextCluster = ScanClusterHeader->NextCluster;
        }
        UpdateClusterHeader(lastHeader, ScanClusterHeader->LastCluster);
        UpdateClusterHeader(nextHeader, ScanClusterHeader->NextCluster);

        UnlockCluster(folderInfo->ClusterHeaderPostition);
        
        free(lastHeader);
        free(nextHeader);
        free(Cluster);   
    }

    bool KFS::UpdatePartitionInfo(){
        return globalPartition->Write(0, sizeof(KFSinfo), KFSPartitionInfo);
    }

    ClusterHeader* KFS::GetClusterHeader(uint64_t cluster){
        if(cluster == 0) return NULL;
        uintptr_t Cluster = calloc(KFSPartitionInfo->ClusterSize);
        ClusterHeader* clusterHeader = (ClusterHeader*)malloc(sizeof(ClusterHeader));
        GetClusterData(cluster, Cluster);
        memcpy(clusterHeader, Cluster, sizeof(ClusterHeader));
        free(Cluster);       
        return clusterHeader;
    }

    HeaderInfo* KFS::GetHeaderInfo(uint64_t cluster){
        if(cluster == 0) return NULL;
        uintptr_t Cluster = calloc(KFSPartitionInfo->ClusterSize);
        HeaderInfo* headerInfo = (HeaderInfo*)malloc(sizeof(HeaderInfo));
        GetClusterData(cluster, Cluster);
        memcpy(headerInfo, Cluster + HeaderInfoPosition, sizeof(HeaderInfo));
        free(Cluster);       
        return headerInfo;
    }

    FolderInfo* KFS::GetParentInfo(FileInfo* fileInfo){
        uintptr_t Cluster = calloc(KFSPartitionInfo->ClusterSize);
        HeaderInfo* ScanHeader = GetHeaderInfo(fileInfo->ClusterHeaderPostition);
        if(ScanHeader->ParentLocationCluster == NULL){
            free(Cluster); 
            free(ScanHeader);      
            return NULL;
        }

        FolderInfo* folderInfo = (FolderInfo*)malloc(sizeof(FolderInfo));
        GetClusterData(ScanHeader->ParentLocationCluster, Cluster);
        memcpy(folderInfo, Cluster + FileInfoPosition, sizeof(FolderInfo));
        free(Cluster); 
        free(ScanHeader);      
        return folderInfo;
    }

    void KFS::UpdateClusterHeader(ClusterHeader* header, uint64_t cluster){
        if(cluster == 0) return;
        uintptr_t Cluster = calloc(KFSPartitionInfo->ClusterSize);
        GetClusterData(cluster, Cluster);
        memcpy(Cluster, header, sizeof(ClusterHeader));
        SetClusterData(cluster, Cluster);
        free(Cluster);       
    }

    void KFS::UpdateFolderInfo(FolderInfo* folderInfo){
        uintptr_t Cluster = calloc(KFSPartitionInfo->ClusterSize);
        GetClusterData(folderInfo->ClusterHeaderPostition, Cluster);
        memcpy(Cluster + FileInfoPosition, folderInfo, sizeof(FolderInfo));
        SetClusterData(folderInfo->ClusterHeaderPostition, Cluster);
        free(Cluster);
    }

    void KFS::UpdateFileInfo(FileInfo* fileInfo){
        uintptr_t Cluster = calloc(KFSPartitionInfo->ClusterSize);
        GetClusterData(fileInfo->ClusterHeaderPostition, Cluster);
        memcpy(Cluster + FileInfoPosition, fileInfo, sizeof(FileInfo));
        SetClusterData(fileInfo->ClusterHeaderPostition, Cluster);
        free(Cluster);
    }

    void KFS::UpdateFileSize(FileInfo* fileInfo, uint64_t newSizeBytes, uint64_t newClusterSize){
        uintptr_t Cluster = calloc(KFSPartitionInfo->ClusterSize);
        int64_t SizeToAddBytes = newSizeBytes - fileInfo->BytesSize;
        int64_t SizeToAddCluster = newClusterSize - fileInfo->ClusterSize;
        fileInfo->BytesSize = newSizeBytes;
        fileInfo->ClusterSize = newClusterSize;

        //Update time file

        RealTimeClock* realTimeClock;
        fileInfo->TimeInfoFS.ModifyTime.seconds = realTimeClock->readSeconds();
        fileInfo->TimeInfoFS.ModifyTime.minutes = realTimeClock->readMinutes();
        fileInfo->TimeInfoFS.ModifyTime.hours = realTimeClock->readHours();
        fileInfo->TimeInfoFS.ModifyTime.days = realTimeClock->readDay();
        fileInfo->TimeInfoFS.ModifyTime.months = realTimeClock->readMonth();
        fileInfo->TimeInfoFS.ModifyTime.years = realTimeClock->readYear() + 2000;

        uint64_t ClusterParents = fileInfo->ClusterHeaderPostition;
        HeaderInfo* ParentsHeader;
        FolderInfo* folderInfo;
        while(true){
            GetClusterData(ClusterParents, Cluster);
            ParentsHeader = (HeaderInfo*)((uint64_t)Cluster + HeaderInfoPosition);
            folderInfo = (FolderInfo*)((uint64_t)Cluster + FileInfoPosition);
            folderInfo->BytesSize += SizeToAddBytes;
            folderInfo->FileClusterSize += SizeToAddCluster;

            //Update time folder

            folderInfo->TimeInfoFS.ModifyTime.seconds = realTimeClock->readSeconds();
            folderInfo->TimeInfoFS.ModifyTime.minutes = realTimeClock->readMinutes();
            folderInfo->TimeInfoFS.ModifyTime.hours = realTimeClock->readHours();
            folderInfo->TimeInfoFS.ModifyTime.days = realTimeClock->readDay();
            folderInfo->TimeInfoFS.ModifyTime.months = realTimeClock->readMonth();
            folderInfo->TimeInfoFS.ModifyTime.years = realTimeClock->readYear() + 2000;

            UpdateFolderInfo(folderInfo);
            
            if(ParentsHeader->ParentLocationCluster == NULL) break;
            ClusterParents = ParentsHeader->ParentLocationCluster;
        }
    }

    void KFS::UpdateFolderSize(FolderInfo* folderInfo, uint64_t newClusterSize){
        uintptr_t Cluster = calloc(KFSPartitionInfo->ClusterSize);
        int64_t SizeToAddCluster = newClusterSize - folderInfo->FileClusterSize;
        folderInfo->FileClusterSize = newClusterSize;

        //Update time file

        RealTimeClock* realTimeClock;
        folderInfo->TimeInfoFS.ModifyTime.seconds = realTimeClock->readSeconds();
        folderInfo->TimeInfoFS.ModifyTime.minutes = realTimeClock->readMinutes();
        folderInfo->TimeInfoFS.ModifyTime.hours = realTimeClock->readHours();
        folderInfo->TimeInfoFS.ModifyTime.days = realTimeClock->readDay();
        folderInfo->TimeInfoFS.ModifyTime.months = realTimeClock->readMonth();
        folderInfo->TimeInfoFS.ModifyTime.years = realTimeClock->readYear() + 2000;

        uint64_t ClusterParents = folderInfo->ClusterHeaderPostition;
        HeaderInfo* ParentsHeader;
        while(true){
            GetClusterData(ClusterParents, Cluster);
            ParentsHeader = (HeaderInfo*)((uint64_t)Cluster + HeaderInfoPosition);
            folderInfo = (FolderInfo*)((uint64_t)Cluster + FileInfoPosition);
            folderInfo->FileClusterSize += SizeToAddCluster;

            //Update time folder

            folderInfo->TimeInfoFS.ModifyTime.seconds = realTimeClock->readSeconds();
            folderInfo->TimeInfoFS.ModifyTime.minutes = realTimeClock->readMinutes();
            folderInfo->TimeInfoFS.ModifyTime.hours = realTimeClock->readHours();
            folderInfo->TimeInfoFS.ModifyTime.days = realTimeClock->readDay();
            folderInfo->TimeInfoFS.ModifyTime.months = realTimeClock->readMonth();
            folderInfo->TimeInfoFS.ModifyTime.years = realTimeClock->readYear() + 2000;

            UpdateFolderInfo(folderInfo);
            
            if(ParentsHeader->ParentLocationCluster == NULL) break;
            ClusterParents = ParentsHeader->ParentLocationCluster;
        }
    }

    uint64_t File::Read(uint64_t start, size_t size, uintptr_t buffer){
        uintptr_t Cluster = calloc(kfs->KFSPartitionInfo->ClusterSize);

        uint64_t ClusterStart = start / kfs->KFSPartitionInfo->ClusterSize;
        uint64_t ClusterCount = 0;
        uint64_t FirstByte = start % kfs->KFSPartitionInfo->ClusterSize;
        if(ClusterStart == 0 && FirstByte < DataPosition){
            ClusterCount = DivideRoundUp(size + DataPosition, kfs->KFSPartitionInfo->ClusterSize - sizeof(ClusterHeader));
        }else{
            ClusterCount = DivideRoundUp(size + FirstByte, kfs->KFSPartitionInfo->ClusterSize - sizeof(ClusterHeader));
        }

        uint64_t ReadCluster = fileInfo.ClusterHeaderPostition;
        uint64_t bytesRead = 0;
        uint64_t bytesToRead = 0;
        uint64_t ClusterRead = 0;

        //find the start Cluster
        for(int i = 0; i < ClusterStart; i++){
            kfs->GetClusterData(ReadCluster, Cluster);
            if(ReadCluster != fileInfo.ClusterHeaderPostition){
                ReadCluster = ((ClusterHeader*)Cluster)->NextCluster;
            }else{
                ReadCluster = fileInfo.NextCluster;
            }

            if(ReadCluster == 0){
                return 0; //end of file before end of read
            }
        }


        while(bytesRead < size){
            if(ReadCluster == 0){
                free(Cluster);
                return 0; //end of file before end of read
            }

            bytesToRead = size - bytesRead;
            if(bytesToRead > kfs->KFSPartitionInfo->ClusterSize){
                bytesToRead = kfs->KFSPartitionInfo->ClusterSize;
            }

            if(bytesToRead > kfs->KFSPartitionInfo->ClusterSize - sizeof(ClusterHeader)){
                bytesToRead -= sizeof(ClusterHeader);
            }


            kfs->GetClusterData(ReadCluster, Cluster);
            
            if(bytesRead != 0){
                memcpy((uintptr_t)((uint64_t)buffer + bytesRead), Cluster + sizeof(ClusterHeader), bytesToRead);
            }else{
                if(ReadCluster == fileInfo.ClusterHeaderPostition){
                    FirstByte += sizeof(HeaderInfo) + sizeof(FileInfo);
                    if(bytesToRead > kfs->KFSPartitionInfo->ClusterSize - FirstByte){
                        bytesToRead -= FirstByte;
                    }
                } 
                memcpy(buffer, (uintptr_t)((uint64_t)Cluster + FirstByte + sizeof(ClusterHeader)), bytesToRead); //Get the correct first byte
            }


            if(ReadCluster != fileInfo.ClusterHeaderPostition){
                ReadCluster = ((ClusterHeader*)Cluster)->NextCluster;
            }else{
                ReadCluster = fileInfo.NextCluster;
            }

            bytesRead += bytesToRead;
        }

        free(Cluster);

        if(ClusterStart + ClusterCount > fileInfo.ClusterSize){
            return 2; //size too big
        }
    }

    uint64_t File::Write(uint64_t start, size_t size, uintptr_t buffer){
        //let's check if we need to enlarge the file or shrink it
        uintptr_t Cluster = calloc(kfs->KFSPartitionInfo->ClusterSize);
        
        uint64_t ClusterStart = 0;
        uint64_t BytesStart = 0;
        if(start != 0){
            ClusterStart = start / (kfs->KFSPartitionInfo->ClusterSize - sizeof(ClusterHeader));
            BytesStart = start % (kfs->KFSPartitionInfo->ClusterSize - sizeof(ClusterHeader));
        }


        uint64_t ClusterCount = 0;
        if(ClusterStart == 0 && BytesStart < DataPosition){
            ClusterCount = DivideRoundUp(size + DataPosition, kfs->KFSPartitionInfo->ClusterSize - sizeof(ClusterHeader));
        }else{
            ClusterCount = DivideRoundUp(size + BytesStart, kfs->KFSPartitionInfo->ClusterSize - sizeof(ClusterHeader));
        }


        uint64_t ClusterTotal = ClusterStart + ClusterCount;
    
        if(ClusterTotal != fileInfo.ClusterSize && ClusterTotal != 0){
            if(ClusterTotal > fileInfo.ClusterSize){
                size_t NewSize = (ClusterTotal - fileInfo.ClusterSize) * kfs->KFSPartitionInfo->ClusterSize - sizeof(ClusterHeader);
                //Alloc new Clusters
                AllocatePartition* allocatePartition;
                if(fileInfo.NextCluster == 0){
                    allocatePartition = kfs->Allocate(NewSize, NULL, 0, false);
                    fileInfo.NextCluster = allocatePartition->FirstCluster;
                }else{
                    allocatePartition = kfs->Allocate(NewSize, NULL, fileInfo.LastClusterOfTheFile, false);
                }
                fileInfo.LastClusterOfTheFile = allocatePartition->LastCluster;
                free(allocatePartition);
                if(fileInfo.ClusterSize == 0) fileInfo.ClusterSize++;
            }else{
                size_t NewSize = ClusterTotal * kfs->KFSPartitionInfo->ClusterSize - sizeof(ClusterHeader);
                //Free last Cluster
                uint64_t ClusterToDelete = fileInfo.ClusterHeaderPostition;
                //find the start Cluster
                for(int i = 0; i < ClusterTotal; i++){
                    kfs->GetClusterData(ClusterToDelete, Cluster);
                    if(ClusterToDelete != fileInfo.ClusterHeaderPostition){
                        ClusterToDelete = ((ClusterHeader*)Cluster)->NextCluster;
                    }else{
                        ClusterToDelete = fileInfo.NextCluster;
                    }
                }   
                kfs->Free(ClusterToDelete);
                if(fileInfo.ClusterSize == 0) fileInfo.ClusterSize++;
            }
        }

        kfs->UpdateFileSize(&fileInfo, start + size, ClusterTotal);
        kfs->UpdateFileInfo(&fileInfo);

        uint64_t FirstByte = start % kfs->KFSPartitionInfo->ClusterSize;
        uint64_t WriteCluster = fileInfo.ClusterHeaderPostition;
        uint64_t bytesWrite = 0;
        uint64_t bytesToWrite = 0;
        uint64_t ClusterWrite = 0;
        
        //find the start Cluster
        for(int i = 0; i < ClusterStart; i++){
            kfs->GetClusterData(WriteCluster, Cluster);
            if(WriteCluster != fileInfo.ClusterHeaderPostition){
                WriteCluster = ((ClusterHeader*)Cluster)->NextCluster;
            }else{
                WriteCluster = fileInfo.NextCluster;
            }

            if(WriteCluster == 0){
                return 0; //end of file before end of read
            }
        }   
        while(bytesWrite < size){
            if(WriteCluster == 0){
                free(Cluster);
                return 0; //end of file before end of write
            }  

            bytesToWrite = size - bytesWrite;
            if(bytesToWrite > kfs->KFSPartitionInfo->ClusterSize){
                bytesToWrite = kfs->KFSPartitionInfo->ClusterSize;
            }

            if(bytesToWrite > kfs->KFSPartitionInfo->ClusterSize - sizeof(ClusterHeader)){
                bytesToWrite -= sizeof(ClusterHeader);
            }

            kfs->GetClusterData(WriteCluster, Cluster);
            if(bytesWrite != 0){
                memcpy(Cluster + sizeof(ClusterHeader), (uintptr_t)((uint64_t)buffer + bytesWrite), bytesToWrite);
            }else{
                if(WriteCluster == fileInfo.ClusterHeaderPostition){
                    FirstByte += sizeof(HeaderInfo) + sizeof(FileInfo);
                    if(bytesToWrite > kfs->KFSPartitionInfo->ClusterSize - FirstByte){
                        bytesToWrite -= FirstByte;
                    }
                }
                memcpy((uintptr_t)((uint64_t)Cluster + FirstByte + sizeof(ClusterHeader)), buffer, bytesToWrite); //Get the correct first byte  
            }
            
            kfs->SetClusterData(WriteCluster, Cluster);
 

            if(WriteCluster != fileInfo.ClusterHeaderPostition){
                WriteCluster = ((ClusterHeader*)Cluster)->NextCluster;
            }else{
                WriteCluster = fileInfo.NextCluster;
            }

            bytesWrite += bytesToWrite;
        }

        free(Cluster);
        return 1;
    }

    void File::Close(){
        free(this);
    }

    uint64_t Folder::GetNumberOfFiles(){
        uint64_t ScanCluster = folderInfo->FirstClusterData;
        if(ScanCluster == 0) return 0;

        uintptr_t Cluster = calloc(kfs->KFSPartitionInfo->ClusterSize);
        ClusterHeader* ScanClusterHeader;
        uint64_t numFiles = 0;

        while(true){
            kfs->GetClusterData(ScanCluster, Cluster);
            ScanClusterHeader = (ClusterHeader*)Cluster;
            numFiles++;            
            if(ScanClusterHeader->NextCluster == NULL) break;
            ScanCluster = ScanClusterHeader->NextCluster;
        }
        free((uintptr_t)Cluster);   

        return numFiles;
    }
}