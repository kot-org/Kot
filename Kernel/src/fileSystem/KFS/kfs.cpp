#include "kfs.h"

namespace FileSystem{

    /* KFS */
    KFS::KFS(GPT::Partition* partition){
        globalPartition = partition;
        KFSPartitionInfo = (KFSinfo*)malloc(sizeof(KFSinfo));
        while(true){
            globalPartition->Read(0, sizeof(KFSinfo), KFSPartitionInfo);
            InitKFS();
            if(KFSPartitionInfo->IsInit.Data1 == GUIDData1 && KFSPartitionInfo->IsInit.Data2 == GUIDData2 && KFSPartitionInfo->IsInit.Data3 == GUIDData3 && KFSPartitionInfo->IsInit.Data4 == GUIDData4) break;  
        }     
    }

    void KFS::InitKFS(){
        uint64_t ClusterSize = 0x10000;
        void* Cluster = malloc(ClusterSize);
        KFSinfo* info = (KFSinfo*)malloc(sizeof(KFSinfo));
        uint64_t MemTotPartiton = (globalPartition->partition->LastLBA - globalPartition->partition->FirstLBA) * globalPartition->port->GetSectorSizeLBA();

        info->IsInit.Data1 = GUIDData1;
        info->IsInit.Data2 = GUIDData2;
        info->IsInit.Data3 = GUIDData3;
        info->IsInit.Data4 = GUIDData4;

        info->NumberOfCluster = MemTotPartiton / ClusterSize;        
        info->BitmapSizeByte = Divide(info->NumberOfCluster, 8);
        info->ClusterSize = ClusterSize;
        info->BitmapSizeCluster = Divide(info->BitmapSizeByte, info->ClusterSize);
        info->BitmapPosition = ClusterSize;
        info->Root.FirstClusterForFile = info->BitmapPosition / ClusterSize + info->BitmapSizeCluster;
        info->Root.FirstClusterFile = 0;
        info->Root.FID = 0;
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
        free((void*)info);
        
    }   


    AllocatePartition* KFS::Allocate(size_t size, Folder* folder, uint64_t LastClusterRequested){
        uint64_t NumberClusterToAllocate = Divide(size, KFSPartitionInfo->ClusterSize);
        uint64_t ClusterAllocate = 1;
        uint64_t FirstBlocAllocated = 0;
        uint64_t NextCluster = 0;
        if(LastClusterRequested == 0){
            LastClusterRequested = KFSPartitionInfo->Root.LastClusterAllocated;
        }        

        if(folder != NULL) LastClusterRequested = folder->folderInfo->LastClusterRequested;
        void* Cluster = malloc(KFSPartitionInfo->ClusterSize);
        void* ClusterLast = malloc(KFSPartitionInfo->ClusterSize);

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
            Free(FirstBlocAllocated, true);
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

    void KFS::Free(uint64_t Cluster, bool DeleteData){
        uint64_t NextBlocToDelete = Cluster;
        void* ClusterData = malloc(KFSPartitionInfo->ClusterSize);
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

    uint64_t KFS::RequestCluster(){
        bool Check = false;
        void* BitmapBuffer = malloc(KFSPartitionInfo->ClusterSize);
        for(int i = 0; i < KFSPartitionInfo->BitmapSizeCluster; i++){
            for(int j = 0; j < KFSPartitionInfo->ClusterSize; j++){
                uint8_t value = *(uint8_t*)((uint64_t)BitmapBuffer + j);
                if(value != uint8_Limit){ //so more than one Cluster is free in this byte
                    for(int y = 0; y < 8; y++){
                        uint64_t Cluster = i * KFSPartitionInfo->ClusterSize + j * 8 + y;
                        if(CheckCluster(Cluster)){ /* is free Cluster */
                            LockCluster(Cluster);
                            free(BitmapBuffer);
                            return Cluster;
                        }
                    }
                }
            }
        } 
        free(BitmapBuffer);
        return 0;
    }

    void KFS::LockCluster(uint64_t Cluster){
        void* BitmapBuffer = malloc(1); 
        globalPartition->Read(KFSPartitionInfo->BitmapPosition + (Cluster / 8), 1, BitmapBuffer);
        uint8_t value = *(uint8_t*)((uint64_t)BitmapBuffer);
        value = WriteBit(value, Cluster % 8, 1);
        *(uint8_t*)((uint64_t)BitmapBuffer) = value;
        globalPartition->Write(KFSPartitionInfo->BitmapPosition + (Cluster / 8), 1, BitmapBuffer);
        free(BitmapBuffer);
    }

    void KFS::UnlockCluster(uint64_t Cluster){
        void* BitmapBuffer = malloc(1); 
        globalPartition->Read(KFSPartitionInfo->BitmapPosition + (Cluster / 8), 1, BitmapBuffer);
        uint8_t value = *(uint8_t*)((uint64_t)BitmapBuffer);
        value = WriteBit(value, Cluster % 8, 0);
        *(uint8_t*)((uint64_t)BitmapBuffer) = value;
        globalPartition->Write(KFSPartitionInfo->BitmapPosition + (Cluster / 8), 1, BitmapBuffer);
        free(BitmapBuffer);
    }

    bool KFS::CheckCluster(uint64_t Cluster){
        bool Check = false;
        void* BitmapBuffer = malloc(1); 
        globalPartition->Read(KFSPartitionInfo->BitmapPosition + (Cluster / 8), 1, BitmapBuffer);
        uint8_t value = *(uint8_t*)((uint64_t)BitmapBuffer);
        
        free(BitmapBuffer); 
        Check = !ReadBit(value, Cluster % 8);
        return Check;
    }

    void KFS::GetClusterData(uint64_t Cluster, void* buffer){
        globalPartition->Read(Cluster * KFSPartitionInfo->ClusterSize, KFSPartitionInfo->ClusterSize, buffer);
    }

    void KFS::SetClusterData(uint64_t Cluster, void* buffer){
        globalPartition->Write(Cluster * KFSPartitionInfo->ClusterSize, KFSPartitionInfo->ClusterSize, buffer);
    }

    void KFS::Close(File* file){
        free(file);
    }

    void KFS::flist(char* filePath){
        char** FoldersSlit = split(filePath, "/");
        int count = 0;
        for(; FoldersSlit[count] != 0; count++);
        count--;

        

        if(KFSPartitionInfo->Root.FirstClusterFile == 0){
            globalLogs->Warning("The disk is empty");
            return;
        }

        void* Cluster = malloc(KFSPartitionInfo->ClusterSize);
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
                        if(strcmp(folderInfo->Name, FoldersSlit[i])){
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

        while(true){
            GetClusterData(ScanCluster, Cluster);
            ScanClusterHeader = (ClusterHeader*)Cluster;
            ScanHeader = (HeaderInfo*)((uint64_t)Cluster + HeaderInfoPosition);
         
            if(ScanHeader->IsFile){
                FileInfo* fileInfo = (FileInfo*)((uint64_t)Cluster + FileInfoPosition);
                globalLogs->Message("File Name : %s FID : %u Cluster : %u\n", fileInfo->Name, ScanHeader->FID, ScanCluster); 
            }else if(ScanHeader->FID != 0){
                FolderInfo* folderInfo = (FolderInfo*)((uint64_t)Cluster + FileInfoPosition);
                globalLogs->Message("Folder Name : %s", folderInfo->Name);
            }
            
            if(ScanClusterHeader->NextCluster == NULL) break;
            ScanCluster = ScanClusterHeader->NextCluster;
        }
        free((void*)Cluster);
    }

    bool KFS::IsDirExist(char* filePath){
        char** FoldersSlit = split(filePath, "/");
        int count = 0;
        for(; FoldersSlit[count] != 0; count++);
        count--;

        

        if(KFSPartitionInfo->Root.FirstClusterFile == 0){
            globalLogs->Warning("The disk is empty");
            return false;
        }

        void* Cluster = malloc(KFSPartitionInfo->ClusterSize);
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
                        if(strcmp(folderInfo->Name, FoldersSlit[i])){
                            ScanCluster = folderInfo->FirstClusterData;
                            break;
                        }
                    }
                    
                    if(ScanClusterHeader->NextCluster == NULL){
                        free((void*)Cluster);
                        return false;
                    } 
                    ScanCluster = ScanClusterHeader->NextCluster;
                }
            }
        }

        free((void*)Cluster);
        return true;
    }

    uint64_t KFS::mkdir(char* filePath, uint64_t Mode){
        if(IsDirExist(filePath)) return 0;
        char** FoldersSlit = split(filePath, "/");
        int count;
        for(count = 0; FoldersSlit[count] != 0; count++);

        void* Cluster = malloc(KFSPartitionInfo->ClusterSize);
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
                    if(strcmp(folderInfo->Name, FoldersSlit[i])){
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

        uint64_t ClusterSize = Divide(DataPosition, KFSPartitionInfo->ClusterSize);
        uint64_t ClusterSizeFolder = KFSPartitionInfo->ClusterSize * ClusterSize;
        AllocatePartition* allocatePartition = Allocate(ClusterSizeFolder, folder, 0);
        
        uint64_t ParentClusterHeaderPostion = 0;
        if(folder != NULL){
            ParentClusterHeaderPostion = folder->folderInfo->ClusterHeaderPostion;
            free(folder);
            free(folder->folderInfo);
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
        HeaderInfo* Header = (HeaderInfo*)(void*)((uint64_t)Cluster + sizeof(ClusterHeader));
        Header->FID = GetFID();
        Header->IsFile = false;
        Header->ParentLocationCluster = ParentClusterHeaderPostion;
        FolderInfo* folderInfo = (FolderInfo*)(void*)((uint64_t)Cluster + sizeof(ClusterHeader) + sizeof(HeaderInfo));
        folderInfo->FirstClusterData = 0;
        folderInfo->BytesSize = 0;
        folderInfo->FileClusterSize = ClusterSize;
        folderInfo->ClusterHeaderPostion = ClusterPosition;

        memcpy(folderInfo->Path, filePath, MaxPath);
        memcpy(folderInfo->Name, FoldersSlit[count - 1], MaxName);

        RealTimeClock* realTimeClock;
        folderInfo->TimeInfoFS.CreateTime.seconds = realTimeClock->readSeconds();
        folderInfo->TimeInfoFS.CreateTime.minutes = realTimeClock->readMinutes();
        folderInfo->TimeInfoFS.CreateTime.hours = realTimeClock->readHours();
        folderInfo->TimeInfoFS.CreateTime.days = realTimeClock->readDay();
        folderInfo->TimeInfoFS.CreateTime.months = realTimeClock->readMonth();
        folderInfo->TimeInfoFS.CreateTime.years = realTimeClock->readYear() + 2000;

        folderInfo->Mode = Mode;

        memcpy((void*)((uint64_t)Cluster + HeaderInfoPosition), Header, sizeof(HeaderInfo));
        memcpy((void*)((uint64_t)Cluster + FileInfoPosition), folderInfo, sizeof(FolderInfo));
        
        SetClusterData(ClusterPosition, Cluster);
        
        free(Cluster);
        return 1; //sucess
    }

    Folder* KFS::readdir(char* filePath){
        char** FoldersSlit = split(filePath, "/");
        int count = 0;
        for(; FoldersSlit[count] != 0; count++);
        count--;

        if(KFSPartitionInfo->Root.FirstClusterFile == 0){
            globalLogs->Warning("The disk is empty");
            return NULL;
        }

        void* Cluster = malloc(KFSPartitionInfo->ClusterSize);
        uint64_t ScanCluster = KFSPartitionInfo->Root.FirstClusterFile;
        ClusterHeader* ScanClusterHeader;
        HeaderInfo* ScanHeader;

        Folder* folder = NULL;

        for(int i = 0; i <= count; i++){
            while(true){
                GetClusterData(ScanCluster, Cluster);
                ScanClusterHeader = (ClusterHeader*)Cluster;
                ScanHeader = (HeaderInfo*)((uint64_t)Cluster + HeaderInfoPosition);
             
                if(!ScanHeader->IsFile && ScanHeader->FID != 0){
                    FolderInfo* folderInfo = (FolderInfo*)((uint64_t)Cluster + FileInfoPosition);
                    if(strcmp(folderInfo->Name, FoldersSlit[i])){
                        ScanCluster = folderInfo->FirstClusterData;
                        if(i == count){
                            folder = (Folder*)malloc(sizeof(Folder));
                            folder->folderInfo = (FolderInfo*)malloc(sizeof(FolderInfo));
                            folder->kfs = this;
                            memcpy(folder->folderInfo, folderInfo, sizeof(FolderInfo));
                            free((void*)Cluster);
                            return folder;
                        }

                        if(ScanCluster == 0){
                            free((void*)Cluster);
                            return NULL;
                        }
                    }
                }

                ScanCluster = ScanClusterHeader->NextCluster;
                if(ScanCluster == 0){
                    free((void*)Cluster);
                    return NULL;
                }
            }
        }
        free((void*)Cluster);
        return folder;
    }

    File* KFS::fopen(char* filePath, char* Mode){
        char** FoldersSlit = split(filePath, "/");
        int count = 0;
        for(; FoldersSlit[count] != 0; count++);
        count--;
        void* Cluster = malloc(KFSPartitionInfo->ClusterSize);
        char* FileName;
        char* FileNameSearch = FoldersSlit[count];
        uint64_t ScanCluster = KFSPartitionInfo->Root.FirstClusterFile;
        ClusterHeader* ScanClusterHeader;
        HeaderInfo* ScanHeader;

        Folder* folder = NULL;

        File* returnData = NULL;

        if(KFSPartitionInfo->Root.FirstClusterFile == 0 && count == 0){
            free((void*)Cluster);
            returnData = (File*)malloc(sizeof(File));
            returnData->fileInfo = NewFile(filePath, folder);
            returnData->Mode = Mode;
            returnData->kfs = this;
            return returnData;
        }else if(KFSPartitionInfo->Root.FirstClusterFile == 0 && count != 0){
            free((void*)Cluster);
            return NULL;
        }

        for(int i = 0; i <= count; i++){
            while(true){
                GetClusterData(ScanCluster, Cluster);
                ScanClusterHeader = (ClusterHeader*)Cluster;
                ScanHeader = (HeaderInfo*)((uint64_t)Cluster + sizeof(ClusterHeader));
             
                if(ScanHeader->IsFile){
                    FileInfo* fileInfo = (FileInfo*)((uint64_t)Cluster + sizeof(ClusterHeader) + sizeof(HeaderInfo));
                    FileName = fileInfo->Name;
        
                    if(i == count && strcmp(FileName, FoldersSlit[count])){
                        returnData = (File*)malloc(sizeof(File));
                        returnData->fileInfo = (FileInfo*)malloc(sizeof(FileInfo));
                        memcpy(returnData->fileInfo, fileInfo, sizeof(FileInfo));
                        returnData->Mode = Mode;
                        returnData->kfs = this;
                        free((void*)Cluster);
                        if(folder != NULL){
                            free((void*)folder);
                        }
                        return returnData;
                    }
                }else if(ScanHeader->FID != 0){
                    FolderInfo* folderInfo = (FolderInfo*)((uint64_t)Cluster + sizeof(ClusterHeader) + sizeof(HeaderInfo));
                    FileName = folderInfo->Name;                    
                    
                    if(i == count){
                        returnData = NULL;
                    }

                    if(strcmp(FileName, FoldersSlit[i])){
                        ScanCluster = folderInfo->FirstClusterData;
                        if(folder != NULL){
                            free((void*)folder->folderInfo);
                            free((void*)folder);
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
                    returnData = NULL;
                    if(i == count){
                        free((void*)Cluster);
                        returnData = (File*)malloc(sizeof(File));
                        returnData->fileInfo = NewFile(filePath, folder);
                        returnData->Mode = Mode;
                        returnData->kfs = this;
                        return returnData;
                    }
                }else{
                    returnData = NULL;
                }
            }            
        } 

        if(folder != NULL){
            free((void*)folder);
        }
        free((void*)Cluster);
        return returnData;
    }

    FileInfo* KFS::NewFile(char* filePath, Folder* folder){
        globalLogs->Message("New file: %s\n", filePath);
        uint64_t ClusterSize = Divide(DataPosition, KFSPartitionInfo->ClusterSize);
        uint64_t FileClusterSize = KFSPartitionInfo->ClusterSize * ClusterSize; //alloc one bloc, for the header and data
        AllocatePartition* allocatePartition = Allocate(FileClusterSize, folder, 0);
        uint64_t ClusterLastAllocate = allocatePartition->LastCluster;
        uint64_t ClusterPosition = allocatePartition->FirstCluster; 
        free(allocatePartition);
        if(KFSPartitionInfo->Root.FirstClusterFile == 0){
            KFSPartitionInfo->Root.FirstClusterFile = ClusterPosition;
        }
        
        void* Cluster = calloc(KFSPartitionInfo->ClusterSize);
        GetClusterData(ClusterPosition, Cluster);
        HeaderInfo* Header = (HeaderInfo*)(void*)((uint64_t)Cluster + sizeof(ClusterHeader));
        Header->FID = GetFID();
        Header->IsFile = true;
        if(folder != NULL){
            Header->ParentLocationCluster = folder->folderInfo->ClusterHeaderPostion;
        }else{
            Header->ParentLocationCluster = 0;
        }

        if(folder != NULL){
            Header->ParentLocationCluster = folder->folderInfo->ClusterHeaderPostion;
        }
        FileInfo* fileInfo = (FileInfo*)(void*)((uint64_t)Cluster + sizeof(ClusterHeader) + sizeof(HeaderInfo));
        fileInfo->BytesSize = DataPosition;
        fileInfo->ClusterSize = ClusterSize;
        fileInfo->LastClusterOfTheFile = ClusterLastAllocate;
        fileInfo->ClusterHeaderPostition = ClusterPosition;
        fileInfo->NextCluster = 0;

        for(int i = 0; i < MaxPath; i++){
            fileInfo->Path[i] = filePath[i];
        }
        

        char** FoldersSlit = split(filePath, "/");
        int count;
        for(count = 0; FoldersSlit[count] != 0; count++);
        for(int i = 0; i < MaxName; i++){
            fileInfo->Name[i] = FoldersSlit[count - 1][i];
        }

        RealTimeClock* realTimeClock;
        fileInfo->TimeInfoFS.CreateTime.seconds = realTimeClock->readSeconds();
        fileInfo->TimeInfoFS.CreateTime.minutes = realTimeClock->readMinutes();
        fileInfo->TimeInfoFS.CreateTime.hours = realTimeClock->readHours();
        fileInfo->TimeInfoFS.CreateTime.days = realTimeClock->readDay();
        fileInfo->TimeInfoFS.CreateTime.months = realTimeClock->readMonth();
        fileInfo->TimeInfoFS.CreateTime.years = realTimeClock->readYear() + 2000;

        memcpy((void*)((uint64_t)Cluster + HeaderInfoPosition), Header, sizeof(HeaderInfo));
        memcpy((void*)((uint64_t)Cluster + FileInfoPosition), fileInfo, sizeof(FileInfo));
        SetClusterData(ClusterPosition, Cluster);
        fileInfo = (FileInfo*)malloc(sizeof(FileInfo));
        memcpy(fileInfo, (void*)((uint64_t)Cluster + FileInfoPosition), sizeof(FileInfo));
        free(Cluster);

        return fileInfo;
    }

    uint64_t KFS::GetFID(){
        KFSPartitionInfo->Root.FID++;
        while(!UpdatePartitionInfo());
        return KFSPartitionInfo->Root.FID;
    }

    bool KFS::UpdatePartitionInfo(){
        return globalPartition->Write(0, sizeof(KFSinfo), KFSPartitionInfo);
    }

    void KFS::UpdateFolderInfo(FolderInfo* folderInfo){
        void* Cluster = calloc(KFSPartitionInfo->ClusterSize);
        GetClusterData(folderInfo->ClusterHeaderPostion, Cluster);
        memcpy(Cluster + FileInfoPosition, folderInfo, sizeof(FolderInfo));
        SetClusterData(folderInfo->ClusterHeaderPostion, Cluster);
        free(Cluster);
    }

    void KFS::UpdateFileInfo(FileInfo* fileInfo){
        void* Cluster = calloc(KFSPartitionInfo->ClusterSize);
        GetClusterData(fileInfo->ClusterHeaderPostition, Cluster);
        memcpy(Cluster + FileInfoPosition, fileInfo, sizeof(FileInfo));
        SetClusterData(fileInfo->ClusterHeaderPostition, Cluster);
        free(Cluster);
    }

    uint64_t File::Read(uint64_t start, size_t size, void* buffer){
        void* Cluster = calloc(kfs->KFSPartitionInfo->ClusterSize);

        uint64_t ClusterStart = start / kfs->KFSPartitionInfo->ClusterSize;
        uint64_t ClusterCount = 0;
        uint64_t FirstByte = start % kfs->KFSPartitionInfo->ClusterSize;
        if(ClusterStart == 0 && FirstByte < DataPosition){
            ClusterCount = Divide(size + DataPosition, kfs->KFSPartitionInfo->ClusterSize - sizeof(ClusterHeader));
        }else{
            ClusterCount = Divide(size + FirstByte, kfs->KFSPartitionInfo->ClusterSize - sizeof(ClusterHeader));
        }

        uint64_t ReadCluster = fileInfo->ClusterHeaderPostition;
        uint64_t bytesRead = 0;
        uint64_t bytesToRead = 0;
        uint64_t ClusterRead = 0;

        //find the start Cluster
        for(int i = 0; i < ClusterStart; i++){
            kfs->GetClusterData(ReadCluster, Cluster);
            if(ReadCluster != fileInfo->ClusterHeaderPostition){
                ReadCluster = ((ClusterHeader*)Cluster)->NextCluster;
            }else{
                ReadCluster = fileInfo->NextCluster;
            }

            if(ReadCluster == 0){
                return 0; //end of file before end of read
            }
        }


        while(bytesRead < size){
            if(ReadCluster == 0){
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
                memcpy((void*)((uint64_t)buffer + bytesRead), Cluster + sizeof(ClusterHeader), bytesToRead);
            }else{
                if(ReadCluster == fileInfo->ClusterHeaderPostition){
                    FirstByte += sizeof(HeaderInfo) + sizeof(FileInfo);
                    if(bytesToRead > kfs->KFSPartitionInfo->ClusterSize - FirstByte){
                        bytesToRead -= FirstByte;
                    }
                } 
                memcpy(buffer, (void*)((uint64_t)Cluster + FirstByte + sizeof(ClusterHeader)), bytesToRead); //Get the correct first byte
            }

            if(ReadCluster != fileInfo->ClusterHeaderPostition){
                ReadCluster = ((ClusterHeader*)Cluster)->NextCluster;
            }else{
                ReadCluster = fileInfo->NextCluster;
            }

            bytesRead += bytesToRead;
        }

        free(Cluster);

        if(ClusterStart + ClusterCount > fileInfo->ClusterSize){
            return 2; //size too big
        }
    }

    uint64_t File::Write(uint64_t start, size_t size, void* buffer){
        //let's check if we need to enlarge the file or shrink it
        void* Cluster = calloc(kfs->KFSPartitionInfo->ClusterSize);
        
        uint64_t ClusterStart = 0;
        uint64_t BytesStart = 0;
        if(start != 0){
            ClusterStart = start / (kfs->KFSPartitionInfo->ClusterSize - sizeof(ClusterHeader));
            BytesStart = start % (kfs->KFSPartitionInfo->ClusterSize - sizeof(ClusterHeader));
        }


        uint64_t ClusterCount = 0;
        if(ClusterStart == 0 && BytesStart < DataPosition){
            ClusterCount = Divide(size + DataPosition, kfs->KFSPartitionInfo->ClusterSize - sizeof(ClusterHeader));
        }else{
            ClusterCount = Divide(size + BytesStart, kfs->KFSPartitionInfo->ClusterSize - sizeof(ClusterHeader));
        }


        uint64_t ClusterTotal = ClusterStart + ClusterCount;
        fileInfo->BytesSize = start + size;
        if(ClusterTotal != fileInfo->ClusterSize && ClusterTotal != 0){
            if(ClusterTotal > fileInfo->ClusterSize){
                size_t NewSize = (ClusterTotal - fileInfo->ClusterSize) * kfs->KFSPartitionInfo->ClusterSize - sizeof(ClusterHeader);
                //Alloc new Clusters
                AllocatePartition* allocatePartition;
                if(fileInfo->NextCluster == 0){
                    allocatePartition = kfs->Allocate(NewSize, NULL, 0);
                    fileInfo->NextCluster = allocatePartition->FirstCluster;
                }else{
                    allocatePartition = kfs->Allocate(NewSize, NULL, fileInfo->LastClusterOfTheFile);
                }
                fileInfo->LastClusterOfTheFile = allocatePartition->LastCluster;
                free(allocatePartition);
                fileInfo->ClusterSize = ClusterTotal;
                if(fileInfo->ClusterSize == 0) fileInfo->ClusterSize++;
            }else{
                size_t NewSize = ClusterTotal * kfs->KFSPartitionInfo->ClusterSize - sizeof(ClusterHeader);
                //Free last Cluster
                uint64_t ClusterToDelete = fileInfo->ClusterHeaderPostition;
                //find the start Cluster
                for(int i = 0; i < ClusterTotal; i++){
                    kfs->GetClusterData(ClusterToDelete, Cluster);
                    if(ClusterToDelete != fileInfo->ClusterHeaderPostition){
                        ClusterToDelete = ((ClusterHeader*)Cluster)->NextCluster;
                    }else{
                        ClusterToDelete = fileInfo->NextCluster;
                    }
                }   
                kfs->Free(ClusterToDelete, true);
                fileInfo->ClusterSize = ClusterTotal;
                if(fileInfo->ClusterSize == 0) fileInfo->ClusterSize++;
            }
        }

        kfs->UpdateFileInfo(fileInfo);

        uint64_t FirstByte = start % kfs->KFSPartitionInfo->ClusterSize;
        uint64_t WriteCluster = fileInfo->ClusterHeaderPostition;
        uint64_t bytesWrite = 0;
        uint64_t bytesToWrite = 0;
        uint64_t ClusterWrite = 0;
        
        //find the start Cluster
        for(int i = 0; i < ClusterStart; i++){
            kfs->GetClusterData(WriteCluster, Cluster);
            if(WriteCluster != fileInfo->ClusterHeaderPostition){
                WriteCluster = ((ClusterHeader*)Cluster)->NextCluster;
            }else{
                WriteCluster = fileInfo->NextCluster;
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
                memcpy(Cluster + sizeof(ClusterHeader), (void*)((uint64_t)buffer + bytesWrite), bytesToWrite);
            }else{
                if(WriteCluster == fileInfo->ClusterHeaderPostition){
                    FirstByte += sizeof(HeaderInfo) + sizeof(FileInfo);
                    if(bytesToWrite > kfs->KFSPartitionInfo->ClusterSize - FirstByte){
                        bytesToWrite -= FirstByte;
                    }
                }
                memcpy((void*)((uint64_t)Cluster + sizeof(ClusterHeader) + FirstByte), buffer, bytesToWrite); //Get the correct first byte
            }

            kfs->SetClusterData(WriteCluster, Cluster);

            if(WriteCluster != fileInfo->ClusterHeaderPostition){
                WriteCluster = ((ClusterHeader*)Cluster)->NextCluster;
            }else{
                WriteCluster = fileInfo->NextCluster;
            }

            bytesWrite += bytesToWrite;
        }

        free(Cluster);
        return 1;
    }
}