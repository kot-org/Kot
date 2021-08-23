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
        uint64_t ClusterSize = 0x10000;
        void* Cluster = malloc(ClusterSize);
        KFSinfo* info = (KFSinfo*)malloc(sizeof(KFSinfo));
        uint64_t MemTotPartiton = (globalPartition->partition->LastLBA - globalPartition->partition->FirstLBA) * globalPartition->port->GetSectorSizeLBA();

        info->IsInit.Data1 = GUIDData1;
        info->IsInit.Data2 = GUIDData2;
        info->IsInit.Data3 = GUIDData3;
        info->IsInit.Data4 = GUIDData4;

        info->numberOfCluster = MemTotPartiton / ClusterSize;        
        info->bitmapSizeByte = Divide(info->numberOfCluster, 8);
        info->ClusterSize = ClusterSize;
        info->bitmapSizeCluster = Divide(info->bitmapSizeByte, info->ClusterSize);
        info->bitmapPosition = ClusterSize;
        info->root.firstClusterForFile = info->bitmapPosition / ClusterSize + info->bitmapSizeCluster;
        info->root.firstClusterFile = 0;
        info->root.fid = 0;
        globalPartition->Write(0, sizeof(KFSinfo), info);


        /* Clear Bitmap */
        memset(Cluster, 0, info->ClusterSize);
        
        

        for(int i = 0; i < info->bitmapSizeCluster; i++){
            globalPartition->Write(info->bitmapPosition + (i * info->ClusterSize), info->ClusterSize, Cluster);
        }

        /* Lock KFSInfo */
        for(int i = 0; i < info->root.firstClusterForFile; i++){
            LockCluster(i);
        }

        free(Cluster);        
        free((void*)info);
        
        //reload info
        globalPartition->Read(0, sizeof(KFSinfo), KFSPartitionInfo);
    }   


    AllocatePartition* KFS::Allocate(size_t size, Folder* folder, uint64_t lastClusterRequested){
        uint64_t NumberClusterToAllocate = Divide(size, KFSPartitionInfo->ClusterSize);
        uint64_t ClusterAllocate = 0;
        uint64_t FirstBlocAllocated = 0;
        uint64_t NextCluster = 0;
        if(lastClusterRequested == 0){
            lastClusterRequested = KFSPartitionInfo->root.lastClusterAllocated;
        }        

        if(folder != NULL) lastClusterRequested = folder->folderInfo->lastClusterRequested;
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
                ClusterHeaderMain->LastCluster = lastClusterRequested;
                ClusterHeaderMain->NextCluster = 0;
                if(lastClusterRequested != 0){
                    GetClusterData(lastClusterRequested, ClusterLast);
                    ClusterHeader* ClusterHeaderLast = (ClusterHeader*)ClusterLast;
                    ClusterHeaderLast->NextCluster = ClusterRequested;
                    memcpy(ClusterLast, ClusterHeaderLast, sizeof(ClusterHeader));
                    SetClusterData(lastClusterRequested, ClusterLast);
                }
                
                memcpy(Cluster, ClusterHeaderMain, sizeof(ClusterHeader));
                SetClusterData(ClusterRequested, Cluster);
                lastClusterRequested = ClusterRequested;                 
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
            KFSPartitionInfo->root.lastClusterAllocated = FirstBlocAllocated;
            UpdatePartitionInfo();
        }else{
            folder->folderInfo->lastClusterRequested = FirstBlocAllocated;
            UpdateFolderInfo(folder);
        }

        free(Cluster);
        free(ClusterLast);
        AllocatePartition* allocatePartition = (AllocatePartition*)malloc(sizeof(AllocatePartition));
        allocatePartition->FirstCluster = FirstBlocAllocated; 
        allocatePartition->LastCluster = lastClusterRequested;
        return allocatePartition;
    }

    void KFS::Free(uint64_t Cluster, bool DeleteData){
        uint64_t NextBlocToDelete = Cluster;
        ClusterHeader* ClusterHeaderToDelete = (ClusterHeader*)malloc(sizeof(ClusterHeader));
        void* BlankBuffer = malloc(KFSPartitionInfo->ClusterSize);
        memset(BlankBuffer, 0, KFSPartitionInfo->ClusterSize);
        while(true){
            GetClusterData(NextBlocToDelete, ClusterHeaderToDelete);                
            uint64_t temp = NextBlocToDelete;
            NextBlocToDelete = ClusterHeaderToDelete->NextCluster;
            if(DeleteData){
               SetClusterData(temp, BlankBuffer); 
            }
            
            UnlockCluster(temp);

            if(NextBlocToDelete == 0){
                break;
            }
        }

        free(BlankBuffer);        
        free(ClusterHeaderToDelete);
    }

    uint64_t KFS::RequestCluster(){
        bool Check = false;
        void* BitmapBuffer = malloc(KFSPartitionInfo->ClusterSize);
        for(int i = 0; i < KFSPartitionInfo->bitmapSizeCluster; i++){
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
        globalPartition->Read(KFSPartitionInfo->bitmapPosition + (Cluster / 8), 1, BitmapBuffer);
        uint8_t value = *(uint8_t*)((uint64_t)BitmapBuffer);
        value = WriteBit(value, Cluster % 8, 1);
        *(uint8_t*)((uint64_t)BitmapBuffer) = value;
        globalPartition->Write(KFSPartitionInfo->bitmapPosition + (Cluster / 8), 1, BitmapBuffer);
        free(BitmapBuffer);
    }

    void KFS::UnlockCluster(uint64_t Cluster){
        void* BitmapBuffer = malloc(1); 
        globalPartition->Read(KFSPartitionInfo->bitmapPosition + (Cluster / 8), 1, BitmapBuffer);
        uint8_t value = *(uint8_t*)((uint64_t)BitmapBuffer);
        value = WriteBit(value, Cluster % 8, 0);
        *(uint8_t*)((uint64_t)BitmapBuffer) = value;
        globalPartition->Write(KFSPartitionInfo->bitmapPosition + (Cluster / 8), 1, BitmapBuffer);
        free(BitmapBuffer);
    }

    bool KFS::CheckCluster(uint64_t Cluster){
        bool Check = false;
        void* BitmapBuffer = malloc(1); 
        globalPartition->Read(KFSPartitionInfo->bitmapPosition + (Cluster / 8), 1, BitmapBuffer);
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

        

        if(KFSPartitionInfo->root.firstClusterFile == 0){
            globalLogs->Warning("The disk is empty");
            return;
        }

        void* Cluster = malloc(KFSPartitionInfo->ClusterSize);
        uint64_t ScanCluster = KFSPartitionInfo->root.firstClusterFile;
        ClusterHeader* ScanClusterHeader;
        HeaderInfo* ScanHeader;

        for(int i = 0; i <= count; i++){
            while(true){
                GetClusterData(ScanCluster, Cluster);
                ScanClusterHeader = (ClusterHeader*)Cluster;
                ScanHeader = (HeaderInfo*)((uint64_t)Cluster + sizeof(ClusterHeader));
             
                if(ScanHeader->IsFile){
                    FileInfo* fileInfo = (FileInfo*)((uint64_t)Cluster + sizeof(ClusterHeader) + sizeof(HeaderInfo));
                    globalLogs->Message("File name : %s FID : %u Cluster : %u\n", fileInfo->name, ScanHeader->FID, ScanCluster); 
                    globalGraphics->Update();
                }else if(ScanHeader->FID != 0){
                    FolderInfo* folderInfo = (FolderInfo*)((uint64_t)Cluster + sizeof(ClusterHeader) + sizeof(HeaderInfo));
                    globalLogs->Message("Folder name : %s", folderInfo->name);
                }

                ScanCluster = ScanClusterHeader->NextCluster;
                if(ScanCluster == 0){
                    break;
                }
            }
        }
        free((void*)Cluster);
    }

    uint64_t KFS::mkdir(char* filePath, uint64_t mode){
        Folder* folder = readdir(filePath);
        char** FoldersSlit = split(filePath, "/");
        int count;
        for(count = 0; FoldersSlit[count] != 0; count++);

        if(count != 0){ //if it isn't root
            if(folder->folderInfo != NULL){
                if(strcmp(folder->folderInfo->name, FoldersSlit[count])) return 0; //folder already exist
                if(!strcmp(folder->folderInfo->name, FoldersSlit[count - 1])) return 2; //folder before doesn't exist 
            }else{
                return 2; //folder before doesn't exist 
            }
        }
        uint64_t ClusterSize = 2;
        uint64_t ClusterSizeFolder = KFSPartitionInfo->ClusterSize * ClusterSize; //alloc one bloc, for the header and data
        uint64_t ClusterPosition = Allocate(ClusterSizeFolder, folder, 0)->FirstCluster; 
        if(KFSPartitionInfo->root.firstClusterFile == 0){
            KFSPartitionInfo->root.firstClusterFile = ClusterPosition;
            UpdatePartitionInfo();
        }
        void* Cluster = malloc(KFSPartitionInfo->ClusterSize);
        GetClusterData(ClusterPosition, Cluster);
        HeaderInfo* Header = (HeaderInfo*)(void*)((uint64_t)Cluster + sizeof(ClusterHeader));
        Header->FID = GetFID();
        Header->IsFile = true;
        FolderInfo* folderInfo = (FolderInfo*)(void*)((uint64_t)Cluster + sizeof(ClusterHeader) + sizeof(HeaderInfo));
        folderInfo->firstClusterData = ((ClusterHeader*)Cluster)->NextCluster;
        folderInfo->size = 0;
        folderInfo->FileClusterSize = ClusterSize;

        for(int i = 0; i < MaxPath; i++){
            folderInfo->path[i] = filePath[i];
        }
        
        for(int i = 0; i < MaxName; i++){
            folderInfo->name[i] = FoldersSlit[count - 1][i];
        }

        RealTimeClock* realTimeClock;
        folderInfo->timeInfoFS.CreateTime.seconds = realTimeClock->readSeconds();
        folderInfo->timeInfoFS.CreateTime.minutes = realTimeClock->readMinutes();
        folderInfo->timeInfoFS.CreateTime.hours = realTimeClock->readHours();
        folderInfo->timeInfoFS.CreateTime.days = realTimeClock->readDay();
        folderInfo->timeInfoFS.CreateTime.months = realTimeClock->readMonth();
        folderInfo->timeInfoFS.CreateTime.years = realTimeClock->readYear() + 2000;

        folderInfo->mode = mode;

        memcpy((void*)((uint64_t)Cluster + sizeof(ClusterHeader)), Header, sizeof(HeaderInfo));
        memcpy((void*)((uint64_t)Cluster + sizeof(ClusterHeader) + sizeof(HeaderInfo)), folderInfo, sizeof(FolderInfo));
        
        SetClusterData(ClusterPosition, Cluster);
        
        free(Cluster);
        return 1; //sucess
    }

    Folder* KFS::readdir(char* filePath){
        char** FoldersSlit = split(filePath, "/");
        int count = 0;
        for(; FoldersSlit[count] != 0; count++);
        count--;

        if(KFSPartitionInfo->root.firstClusterFile == 0){
            printf("The disk is empty");
            return NULL;
        }

        void* Cluster = malloc(KFSPartitionInfo->ClusterSize);
        uint64_t ScanCluster = KFSPartitionInfo->root.firstClusterFile;
        ClusterHeader* ScanClusterHeader;
        HeaderInfo* ScanHeader;

        Folder* folder = NULL;

        for(int i = 0; i <= count; i++){
            while(true){
                GetClusterData(ScanCluster, Cluster);
                ScanClusterHeader = (ClusterHeader*)Cluster;
                ScanHeader = (HeaderInfo*)((uint64_t)Cluster + sizeof(ClusterHeader));
             
                if(!ScanHeader->IsFile && ScanHeader->FID != 0){
                    FolderInfo* folderInfo = (FolderInfo*)((uint64_t)Cluster + sizeof(ClusterHeader) + sizeof(HeaderInfo));
                    if(strcmp(folderInfo->name, FoldersSlit[i])){
                        if(folder != NULL){
                            free((void*)folder);
                        }
                        folder = (Folder*)malloc(sizeof(Folder));
                    }
                }

                ScanCluster = ScanClusterHeader->NextCluster;
                if(ScanCluster == 0){
                    break;
                }
            }
        }
        free((void*)Cluster);
        return folder;
    }

    File* KFS::fopen(char* filePath, char* mode){
        char** FoldersSlit = split(filePath, "/");
        int count = 0;
        for(; FoldersSlit[count] != 0; count++);
        count--;
        void* Cluster = malloc(KFSPartitionInfo->ClusterSize);
        char* FileName;
        char* FileNameSearch = FoldersSlit[count];
        uint64_t ScanCluster = KFSPartitionInfo->root.firstClusterFile;
        ClusterHeader* ScanClusterHeader;
        HeaderInfo* ScanHeader;

        Folder* folder = NULL;

        File* returnData = NULL;

        if(KFSPartitionInfo->root.firstClusterFile == 0 && count == 0){
            free((void*)Cluster);
            returnData = (File*)malloc(sizeof(File));
            returnData->fileInfo = NewFile(filePath, folder);
            returnData->mode = mode;
            returnData->kfs = this;
            return returnData;
        }else if(KFSPartitionInfo->root.firstClusterFile == 0 && count != 0){
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
                    FileName = fileInfo->name;
        
                    int FileNameLen = strlen(FileName);
                    int FileNameTempLen = strlen(FoldersSlit[count]);
                    if(i == count && strcmp(FileName, FoldersSlit[count])){
                        free((void*)Cluster);

                        returnData = (File*)malloc(sizeof(File));
                        returnData->fileInfo = fileInfo;
                        returnData->mode = mode;
                        returnData->kfs = this;
                        if(folder != NULL){
                            free((void*)folder);
                        }
                        return returnData;
                    }
                }else if(ScanHeader->FID != 0){
                    FolderInfo* folderInfo = (FolderInfo*)((uint64_t)Cluster + sizeof(ClusterHeader) + sizeof(HeaderInfo));
                    FileName = folderInfo->name;
                    if(i == count){
                        returnData = NULL;
                    }

                    if(FileName == FoldersSlit[i]){
                        ScanCluster = folderInfo->firstClusterData;
                        if(folder != NULL){
                            free((void*)folder);
                        }
                        folder = (Folder*)malloc(sizeof(Folder));
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
                        returnData->mode = mode;
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
        uint64_t ClusterSize = 2;
        uint64_t FileClusterSize = KFSPartitionInfo->ClusterSize * ClusterSize; //alloc one bloc, for the header and data
        AllocatePartition* allocatePartition = Allocate(FileClusterSize, folder, 0);
        uint64_t ClusterLastAllocate = allocatePartition->LastCluster;
        uint64_t ClusterPosition = allocatePartition->FirstCluster; 
        free(allocatePartition);
        if(KFSPartitionInfo->root.firstClusterFile == 0){
            KFSPartitionInfo->root.firstClusterFile = ClusterPosition;
        }
        
        void* Cluster = malloc(KFSPartitionInfo->ClusterSize);
        memset(Cluster, 0, KFSPartitionInfo->ClusterSize);
        GetClusterData(ClusterPosition, Cluster);
        HeaderInfo* Header = (HeaderInfo*)(void*)((uint64_t)Cluster + sizeof(ClusterHeader));
        Header->FID = GetFID();
        Header->IsFile = true;
        Header->ParentLocationCluster = folder->folderInfo->ClusterHeader;
        FileInfo* fileInfo = (FileInfo*)(void*)((uint64_t)Cluster + sizeof(ClusterHeader) + sizeof(HeaderInfo));
        fileInfo->ClusterHeader = ClusterPosition;
        fileInfo->firstClusterData = ((ClusterHeader*)Cluster)->NextCluster;
        fileInfo->size = 0;
        fileInfo->FileClusterSize = ClusterSize;
        fileInfo->FileClusterData = ClusterSize - Divide(sizeof(ClusterHeader), KFSPartitionInfo->ClusterSize); //the header use 1 Cluster
        fileInfo->lastCluster = ClusterLastAllocate;

        for(int i = 0; i < MaxPath; i++){
            fileInfo->path[i] = filePath[i];
        }
        

        char** FoldersSlit = split(filePath, "/");
        int count;
        for(count = 0; FoldersSlit[count] != 0; count++);
        for(int i = 0; i < MaxName; i++){
            fileInfo->name[i] = FoldersSlit[count - 1][i];
        }

        RealTimeClock* realTimeClock;
        fileInfo->timeInfoFS.CreateTime.seconds = realTimeClock->readSeconds();
        fileInfo->timeInfoFS.CreateTime.minutes = realTimeClock->readMinutes();
        fileInfo->timeInfoFS.CreateTime.hours = realTimeClock->readHours();
        fileInfo->timeInfoFS.CreateTime.days = realTimeClock->readDay();
        fileInfo->timeInfoFS.CreateTime.months = realTimeClock->readMonth();
        fileInfo->timeInfoFS.CreateTime.years = realTimeClock->readYear() + 2000;

        memcpy((void*)((uint64_t)Cluster + HeaderInfoPosition), Header, sizeof(HeaderInfo));
        memcpy((void*)((uint64_t)Cluster + FileInfoPosition), fileInfo, sizeof(FileInfo));
        SetClusterData(ClusterPosition, Cluster);
        fileInfo = (FileInfo*)malloc(sizeof(FileInfo));
        memcpy(fileInfo, (void*)((uint64_t)Cluster + sizeof(ClusterHeader) + sizeof(HeaderInfo)), sizeof(FileInfo));
        free(Cluster);
        return fileInfo;
    }

    uint64_t KFS::GetFID(){
        KFSPartitionInfo->root.fid++;
        while(!UpdatePartitionInfo());
        return KFSPartitionInfo->root.fid;
    }

    bool KFS::UpdatePartitionInfo(){
        return globalPartition->Write(0, sizeof(KFSinfo), KFSPartitionInfo);
    }

    void KFS::UpdateFolderInfo(Folder* folder){
        SetClusterData(folder->folderInfo->ClusterHeader, folder->folderInfo);
    }

    void KFS::UpdateFileInfo(FileInfo* fileInfo){
        SetClusterData(fileInfo->ClusterHeader, fileInfo);
    }

    uint64_t File::Read(uint64_t start, size_t size, void* buffer){
        void* Cluster = malloc(kfs->KFSPartitionInfo->ClusterSize);

        uint64_t ClusterStart = start / kfs->KFSPartitionInfo->ClusterSize;
        uint64_t ClusterCount = Divide(size, kfs->KFSPartitionInfo->ClusterSize - sizeof(ClusterHeader));
        
        uint64_t FirstByte = start % kfs->KFSPartitionInfo->ClusterSize;
        uint64_t ReadCluster = fileInfo->firstClusterData;
        uint64_t bytesRead = 0;
        uint64_t bytesToRead = 0;
        uint64_t ClusterRead = 0;

        //find the start Cluster
        for(int i = 0; i < ClusterStart; i++){
            kfs->GetClusterData(ReadCluster, Cluster);

            ReadCluster = ((ClusterHeader*)Cluster)->NextCluster;

            if(ReadCluster == 0){
                return 0; //end of file before end of read
            }
        }

        for(int i = 0; i < ClusterCount; i++){
            globalLogs->Message("Test %u", ReadCluster);
            bytesToRead = size - bytesRead;
            if(bytesToRead > kfs->KFSPartitionInfo->ClusterSize){
                bytesToRead = kfs->KFSPartitionInfo->ClusterSize;
            }

            kfs->GetClusterData(ReadCluster, Cluster);

            ReadCluster = ((ClusterHeader*)Cluster)->NextCluster;

            if(ReadCluster == 0){
                return 0; //end of file before end of read
            }

            
            if(bytesRead != 0){
                memcpy((void*)((uint64_t)buffer + bytesRead), Cluster + sizeof(ClusterHeader), bytesToRead);
            }else{
                memcpy(buffer, (void*)((uint64_t)Cluster + sizeof(ClusterHeader) + FirstByte), bytesToRead); //Get the correct first byte
            }

            bytesRead += kfs->KFSPartitionInfo->ClusterSize;
        }

        free(Cluster);

        if(ClusterStart + ClusterCount > fileInfo->FileClusterSize){
            return 2; //size too big
        }
    }

    uint64_t File::Write(uint64_t start, size_t size, void* buffer){
        //let's check if we need to enlarge the file or shrink it
        void* Cluster = malloc(kfs->KFSPartitionInfo->ClusterSize);
        
        uint64_t ClusterStart = 0;
        if(start != 0){
            ClusterStart = start / (kfs->KFSPartitionInfo->ClusterSize - sizeof(ClusterHeader));
        }
        uint64_t ClusterCount = Divide(size, kfs->KFSPartitionInfo->ClusterSize - sizeof(ClusterHeader));
        uint64_t ClusterTotal = ClusterStart + ClusterCount;

        /*check here for the bug */
        if(ClusterTotal != fileInfo->FileClusterData){
            if(ClusterTotal > fileInfo->FileClusterData){
                size_t NewSize = ClusterTotal * kfs->KFSPartitionInfo->ClusterSize - sizeof(ClusterHeader);
                //Aloc new Clusters
                fileInfo->lastCluster = (kfs->Allocate(NewSize, NULL, fileInfo->lastCluster))->LastCluster;
                fileInfo->size = NewSize;
                fileInfo->FileClusterData = ClusterTotal;
                fileInfo->FileClusterSize = ClusterTotal + 1;
                kfs->UpdateFileInfo(fileInfo);
            }else{
                size_t NewSize = ClusterTotal * kfs->KFSPartitionInfo->ClusterSize - sizeof(ClusterHeader);
                //Free last Cluster
                kfs->Free(fileInfo->firstClusterData + ClusterTotal, true);
                fileInfo->size = NewSize;
                fileInfo->FileClusterSize = ClusterTotal + Divide(sizeof(ClusterHeader), kfs->KFSPartitionInfo->ClusterSize);
                fileInfo->FileClusterData = ClusterTotal;
                kfs->UpdateFileInfo(fileInfo);
            }
        }

        uint64_t FirstByte = start % kfs->KFSPartitionInfo->ClusterSize - sizeof(ClusterHeader);
        uint64_t WriteCluster = fileInfo->firstClusterData;
        uint64_t bytesWrite = 0;
        uint64_t bytesToWrite = 0;
        uint64_t ClusterWrite = 0;

        //find the start Cluster
        for(int i = 0; i < ClusterStart; i++){
            kfs->GetClusterData(WriteCluster, Cluster);

            WriteCluster = ((ClusterHeader*)Cluster)->NextCluster;

            if(WriteCluster == 0){
                return 0; //end of file before end of read
            }
        }

        for(int i = 0; i < ClusterCount; i++){
            bytesToWrite = size - bytesWrite;
            if(bytesToWrite > kfs->KFSPartitionInfo->ClusterSize){
                bytesToWrite = kfs->KFSPartitionInfo->ClusterSize;
            }

            if(bytesWrite != 0){
                memcpy(Cluster, (void*)((uint64_t)buffer + sizeof(ClusterHeader) + bytesWrite), bytesToWrite);
            }else{
                memcpy((void*)((uint64_t)Cluster + sizeof(ClusterHeader) + FirstByte), buffer, bytesToWrite); //Get the correct first byte
            }
            kfs->SetClusterData(WriteCluster, Cluster);

            kfs->GetClusterData(WriteCluster, Cluster);

            WriteCluster = ((ClusterHeader*)Cluster)->NextCluster;

            if(WriteCluster == 0){
                return 0; //end of file before end of read
            }            

            bytesWrite += kfs->KFSPartitionInfo->ClusterSize;
        }

        free(Cluster);
        return 1;
    }
}