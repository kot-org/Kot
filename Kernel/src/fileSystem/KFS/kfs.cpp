#include "kfs.h"

namespace FileSystem{

    /* KFS */
    KFS::KFS(GPT::Partition* partition){
        globalPartition = partition;
        KFSPartitionInfo = (KFSinfo*)mallocK(sizeof(KFSinfo));
        while(true){
            globalPartition->Read(0, sizeof(KFSinfo), KFSPartitionInfo);
            
            InitKFS();
            if(KFSPartitionInfo->IsInit) break;
        }
        
    }

    void KFS::InitKFS(){
        KFSinfo* info = (KFSinfo*)mallocK(sizeof(KFSinfo));
        uint64_t MemTotPartiton = (globalPartition->partition->LastLBA - globalPartition->partition->FirstLBA) * globalPartition->port->GetSectorSizeLBA();
        uint64_t BlockSize = blockSize;
        info->IsInit = true;        
        info->bitmapSizeByte = (MemTotPartiton / BlockSize) / 8;
        info->BlockSize = BlockSize;
        info->bitmapSizeBlock = info->bitmapSizeByte / info->BlockSize + 1;        
        info->numBlock = MemTotPartiton / BlockSize;
        info->bitmapPosition = BlockSize;
        info->firstBlocFile = (info->bitmapPosition / BlockSize) + info->bitmapSizeBlock;
        globalPartition->Write(0, sizeof(KFSinfo), info);

        /*Clear Bitmap*/
        void* FreeBitmap = mallocK(info->BlockSize);
        memset(FreeBitmap, 2, info->BlockSize);

        for(int i = 0; i < info->bitmapSizeBlock; i++){
            globalPartition->Write(info->bitmapPosition + (i * info->BlockSize), info->BlockSize, FreeBitmap);
        }
        /* Lock KFSInfo*/
        for(int i = 0; i < info->firstBlocFile; i++){
            LockBlock(i);
        }

        freeK(FreeBitmap);
        freeK((void*)info);
    }   


    uint64_t KFS::Alloc(size_t size){
        uint64_t NumberBlockToAllocate = size / KFSPartitionInfo->BlockSize;
        uint64_t BlockAllocate = 0;
        uint64_t FirstBlocAllocated = 0;
        uint64_t LastBlock = 0;
        uint64_t NextBlock = 0;

        for(int i = 0; i < KFSPartitionInfo->BlockSize; i++){
            if(CheckBlock(i)){                
                LockBlock(i);
                BlockAllocate++;
                if(FirstBlocAllocated == 0){
                    FirstBlocAllocated = i;
                }
                BlockHeader* blockHeader = (BlockHeader*)mallocK(sizeof(BlockHeader));
                blockHeader->LastBlock = LastBlock;
                blockHeader->NextBlock = 0;

                if(LastBlock != 0){
                    BlockHeader* blockHeaderLast = (BlockHeader*)mallocK(sizeof(BlockHeader));
                    GetBlockData(LastBlock, blockHeaderLast);
                    blockHeaderLast->NextBlock = i;
                    SetBlockData(LastBlock, blockHeaderLast);
                    freeK(blockHeaderLast);
                }
                
                SetBlockData(i, blockHeader);
                freeK(blockHeader);
                LastBlock = i;
            }
        }

        if(BlockAllocate < NumberBlockToAllocate){
            Free(FirstBlocAllocated, true);
            return 0;
        }
        return FirstBlocAllocated;
    }

    void KFS::Free(uint64_t Block, bool DeleteData){
        uint64_t NextBlocToDelete = Block;
        BlockHeader* blockHeaderToDelete = (BlockHeader*)mallocK(sizeof(BlockHeader));
        void* BlankBuffer = mallocK(KFSPartitionInfo->BlockSize);
        memset(BlankBuffer, 0, KFSPartitionInfo->BlockSize);
        while(true){
            GetBlockData(NextBlocToDelete, blockHeaderToDelete);                
            uint64_t temp = NextBlocToDelete;
            NextBlocToDelete = blockHeaderToDelete->NextBlock;
            if(DeleteData){
               SetBlockData(temp, BlankBuffer); 
            }
            
            UnlockBlock(temp);

            if(NextBlocToDelete == 0){
                break;
            }
        }
        freeK(blockHeaderToDelete);
    }

    void KFS::LockBlock(uint64_t Block){
        void* BitmapBuffer = mallocK(1); 
        globalPartition->Read(KFSPartitionInfo->bitmapPosition + (Block / 8), 1, BitmapBuffer);
        uint8_t value = *(uint8_t*)((uint64_t)BitmapBuffer);
        WriteBit(value, Block % 8, 1);
        globalPartition->Write(KFSPartitionInfo->bitmapPosition + (Block / 8), 1, BitmapBuffer);
        freeK(BitmapBuffer);
    }

    void KFS::UnlockBlock(uint64_t Block){
        void* BitmapBuffer = mallocK(1); 
        globalPartition->Read(KFSPartitionInfo->bitmapPosition + (Block / 8), 1, BitmapBuffer);
        uint8_t value = *(uint8_t*)((uint64_t)BitmapBuffer);
        WriteBit(value, Block % 8, 0);
        globalPartition->Write(KFSPartitionInfo->bitmapPosition + (Block / 8), 1, BitmapBuffer);
        freeK(BitmapBuffer);
    } 

    bool KFS::CheckBlock(uint64_t Block){
        bool Check = false;
        void* BitmapBuffer = mallocK(1); 
        globalPartition->Read(KFSPartitionInfo->bitmapPosition + (Block / 8), 1, BitmapBuffer);
        uint8_t value = *(uint8_t*)((uint64_t)BitmapBuffer);
        if(!ReadBit(value, Block % 8)){
            Check = true;
        }else{
            Check = false;
        }
        freeK(BitmapBuffer);
        return Check;
    }

    void KFS::GetBlockData(uint64_t Block, void* buffer){
        globalPartition->Read(Block * KFSPartitionInfo->BlockSize, KFSPartitionInfo->BlockSize, buffer);
    }

    void KFS::SetBlockData(uint64_t Block, void* buffer){
        globalPartition->Write(Block * KFSPartitionInfo->BlockSize, KFSPartitionInfo->BlockSize, buffer);
    }

    void KFS::Close(File* file){

    }

    File* KFS::fopen(char* filename, char* mode){
        char** FoldersSlit = split(filename, "/");
        int count = 0;
        for(; FoldersSlit[count] != 0; count++);
        count--;

        void* Block = mallocK(KFSPartitionInfo->BlockSize);
        char* FileName;
        uint64_t ScanBlock = KFSPartitionInfo->firstBlocFile;
        BlockHeader* ScanBlockHeader;
        HeaderInfo* ScanHeader;

        File* returnData;
        
        for(int i = 0; i <= count; i++){
            while(true){
                printf("%u ", KFSPartitionInfo->firstBlocFile);
                globalGraphics->Update();
                GetBlockData(ScanBlock, Block);
                ScanBlockHeader = (BlockHeader*)Block;
                ScanHeader = (HeaderInfo*)((uint64_t)Block + sizeof(BlockHeader));
                if(ScanHeader->IsFile){
                    FileInfo* fileInfo = (FileInfo*)((uint64_t)Block + sizeof(BlockHeader) + sizeof(HeaderInfo));
                    FileName = fileInfo->name;
                    if(i == count){
                        returnData->fileInfo = fileInfo;
                        returnData->mode = mode;
                        return returnData;
                    }
                }else{
                    /* it can also be nothing */
                    FolderInfo* folderInfo = (FolderInfo*)((uint64_t)Block + sizeof(BlockHeader) + sizeof(HeaderInfo));
                    FileName = folderInfo->name;
                    if(i == count){
                        returnData = NULL;
                    }

                    if(FileName == FoldersSlit[i]){
                        ScanBlock = folderInfo->firstBlock;
                    }
                } 
                if(FileName == FoldersSlit[i]) break;

                ScanBlock = ScanBlockHeader->NextBlock;
                if(ScanBlock == 0){
                    returnData = NULL;
                    if(i == count){
                        returnData->fileInfo = NewFile(filename);
                        returnData->mode = mode;
                        return returnData;
                    }
                }else{
                    returnData = NULL;
                }
            }            
        } 

        freeK((void*)Block);
        return returnData;
    }

    FileInfo* KFS::NewFile(char* filePath){
        uint64_t FileBlockSize = KFSPartitionInfo->BlockSize; //alloc two bloc, one for the struct and other for the data
        uint64_t blocPosition = Alloc(FileBlockSize); 
        void* block = mallocK(KFSPartitionInfo->BlockSize);
        GetBlockData(blocPosition, block);
        HeaderInfo* Header = (HeaderInfo*)block + sizeof(BlockHeader);
        Header->IsFile = true;
        FileInfo* fileInfo = (FileInfo*)block + sizeof(BlockHeader) + sizeof(HeaderInfo);
        fileInfo->firstBlock = ((BlockHeader*)block)->NextBlock;
        fileInfo->size = 0;
        fileInfo->FileBlockSize = FileBlockSize;
        for(int i = 0; i < MaxPath; i++){
            fileInfo->path[i] = *filePath;
            filePath++;
        }
        

        char** FoldersSlit = split(filePath, "/");
        int count;
        for(count = 0; FoldersSlit[count] != 0; count++);
        for(int i = 0; i < MaxName; i++){
            fileInfo->name[i] = *FoldersSlit[count];
            FoldersSlit[count]++;
        }

        RealTimeClock* realTimeClock;
        fileInfo->timeInfoFS.CreateTime.seconds = realTimeClock->readSeconds();
        fileInfo->timeInfoFS.CreateTime.minutes = realTimeClock->readMinutes();
        fileInfo->timeInfoFS.CreateTime.hours = realTimeClock->readHours();
        fileInfo->timeInfoFS.CreateTime.days = realTimeClock->readDay();
        fileInfo->timeInfoFS.CreateTime.months = realTimeClock->readMonth();
        fileInfo->timeInfoFS.CreateTime.years = realTimeClock->readYear() + 2000;
        
        SetBlockData(blocPosition, block);

        
        return fileInfo;
    }
}