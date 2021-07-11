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
        info->numberOfBlock = MemTotPartiton / BlockSize;        
        info->bitmapSizeByte = Divide(info->numberOfBlock, 8);
        info->BlockSize = BlockSize;
        info->bitmapSizeBlock = Divide(info->bitmapSizeByte, info->BlockSize);
        info->bitmapPosition = BlockSize;
        info->firstBlockFile = Divide(info->bitmapPosition, BlockSize) + info->bitmapSizeBlock;
        globalPartition->Write(0, sizeof(KFSinfo), info);

        /*Clear Bitmap*/
        void* FreeBitmap = mallocK(info->BlockSize);
        memset(FreeBitmap, 0, info->BlockSize);
        

        for(int i = 0; i < info->bitmapSizeBlock; i++){
            globalPartition->Write(info->bitmapPosition + (i * info->BlockSize), info->BlockSize, FreeBitmap);
        }
        /* Lock KFSInfo*/
        for(int i = 0; i < info->firstBlockFile; i++){
            LockBlock(i);
        }


        printf("%u", Alloc(0x1000000000));
        memset(FreeBitmap, 0, info->BlockSize);
        freeK(FreeBitmap);
        freeK((void*)info);
    }   


    uint64_t KFS::Alloc(size_t size){
        uint64_t NumberBlockToAllocate = Divide(size, KFSPartitionInfo->BlockSize);
        uint64_t BlockAllocate = 0;
        uint64_t FirstBlocAllocated = 0;
        uint64_t LastBlock = 0;
        uint64_t NextBlock = 0;

        for(int i = 0; i < NumberBlockToAllocate; i++){
            uint64_t BlockRequested = RequestBlock();
            if(BlockRequested != 0){
                BlockAllocate++;

                if(FirstBlocAllocated == 0){ //for the return value
                    FirstBlocAllocated = BlockRequested;
                }
                BlockHeader* blockHeader = (BlockHeader*)mallocK(sizeof(BlockHeader));
                blockHeader->LastBlock = LastBlock;
                blockHeader->NextBlock = 0;

                if(LastBlock != 0){
                    BlockHeader* blockHeaderLast = (BlockHeader*)mallocK(sizeof(BlockHeader));
                    GetBlockData(LastBlock, blockHeaderLast);
                    printf("%u ", blockHeaderLast->NextBlock);

                    blockHeaderLast->NextBlock = BlockRequested;
                    SetBlockData(LastBlock, blockHeaderLast);
                    freeK(blockHeaderLast);
                }
                
                SetBlockData(i, blockHeader);
                freeK(blockHeader);
                LastBlock = i;
            }else{
                break;
            }
        }

        if(BlockAllocate < NumberBlockToAllocate && BlockAllocate > 0){
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

    uint64_t KFS::RequestBlock(){
        bool Check = false;
        void* BitmapBuffer = mallocK(KFSPartitionInfo->BlockSize);
        for(int i = 0; i < KFSPartitionInfo->bitmapSizeBlock; i++){
            for(int j = 0; j < KFSPartitionInfo->BlockSize; j++){
                uint8_t value = *(uint8_t*)((uint64_t)BitmapBuffer + j);
                if(value != uint8_Limit){ //so more than one block is free in this byte
                    for(int y = 0; y < 8; y++){
                        uint64_t Block = i * KFSPartitionInfo->BlockSize + j * 8 + y;
                        if(CheckBlock(Block)){ /* is free block */
                            LockBlock(Block);
                            freeK(BitmapBuffer);
                            return Block;
                        }
                    }
                }
            }
        } 
        freeK(BitmapBuffer);
        return 0;
    }

    void KFS::LockBlock(uint64_t Block){
        void* BitmapBuffer = mallocK(1); 
        globalPartition->Read(KFSPartitionInfo->bitmapPosition + (Block / 8), 1, BitmapBuffer);
        uint8_t value = *(uint8_t*)((uint64_t)BitmapBuffer);
        value = WriteBit(value, Block % 8, 1);
        *(uint8_t*)((uint64_t)BitmapBuffer) = value;
        globalPartition->Write(KFSPartitionInfo->bitmapPosition + (Block / 8), 1, BitmapBuffer);
        freeK(BitmapBuffer);
    }

    void KFS::UnlockBlock(uint64_t Block){
        void* BitmapBuffer = mallocK(1); 
        globalPartition->Read(KFSPartitionInfo->bitmapPosition + (Block / 8), 1, BitmapBuffer);
        uint8_t value = *(uint8_t*)((uint64_t)BitmapBuffer);
        value = WriteBit(value, Block % 8, 0);
        *(uint8_t*)((uint64_t)BitmapBuffer) = value;
        globalPartition->Write(KFSPartitionInfo->bitmapPosition + (Block / 8), 1, BitmapBuffer);
        freeK(BitmapBuffer);
    }

    bool KFS::CheckBlock(uint64_t Block){
        bool Check = false;
        void* BitmapBuffer = mallocK(1); 
        globalPartition->Read(KFSPartitionInfo->bitmapPosition + (Block / 8), 1, BitmapBuffer);
        uint8_t value = *(uint8_t*)((uint64_t)BitmapBuffer);
        freeK(BitmapBuffer);
        return !ReadBit(value, Block % 8);
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
        uint64_t ScanBlock = KFSPartitionInfo->firstBlockFile;
        BlockHeader* ScanBlockHeader;
        HeaderInfo* ScanHeader;

        File* returnData;
        
        for(int i = 0; i <= count; i++){
            while(true){
                printf("%u ", KFSPartitionInfo->firstBlockFile);
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