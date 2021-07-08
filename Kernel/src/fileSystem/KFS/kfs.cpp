#include "kfs.h"

namespace FileSystem{

    /* KFS */
    KFS::KFS(GPT::Partition* partition){
        globalPartition = partition;
        KFSPartitionInfo = (KFSinfo*)mallocK(sizeof(KFSinfo));
        while(true){
            globalPartition->Read(0, sizeof(KFSinfo), KFSPartitionInfo);
            if(KFSPartitionInfo->IsInit) break;
            InitKFS();
        }
        
    }

    void KFS::InitKFS(){
        KFSinfo* info = (KFSinfo*)mallocK(sizeof(KFSinfo));
        uint64_t MemTotPartiton = globalPartition->partition->LastLBA - globalPartition->partition->FirstLBA;
        uint64_t BlockSize = blockSize;
        info->IsInit = true;        
        info->bitmapSizeByte = (MemTotPartiton / BlockSize) / 8;
        info->BlockSize = BlockSize;
        info->bitmapSizeBloc = info->bitmapSizeByte / info->BlockSize;        
        info->numBlock = MemTotPartiton / BlockSize;
        info->bitmapPosition = BlockSize;
        info->firstBlocFile = info->bitmapPosition + info->bitmapSizeBloc;
        globalPartition->Write(0, sizeof(KFSinfo), info);

        /*Clear Bitmap*/
        void* FreeBitmap = mallocK(info->BlockSize);
        memset(FreeBitmap, 0, info->BlockSize);
        for(int i = 0; i < info->bitmapSizeBloc; i++){
            globalPartition->Write(info->bitmapPosition + (i * info->BlockSize), info->BlockSize, FreeBitmap);
        }

        /* Lock KFSInfo*/
        for(int i = 0; i < info->firstBlocFile; i++){
            LockBlock(i);
        }
        free(FreeBitmap);
        free((void*)info);
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
                    free(blockHeaderLast);
                }
                
                SetBlockData(i, blockHeader);
                free(blockHeader);
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
        free(blockHeaderToDelete);
    }

    void KFS::LockBlock(uint64_t Block){
        void* BitmapBuffer = mallocK(1); 
        globalPartition->Read(KFSPartitionInfo->bitmapPosition + (Block / 8), 1, BitmapBuffer);
        uint8_t value = *(uint8_t*)((uint64_t)BitmapBuffer);
        WriteBit(value, Block % 8, 1);
        globalPartition->Write(KFSPartitionInfo->bitmapPosition + (Block / 8), 1, BitmapBuffer);
        free(BitmapBuffer);
    }

    void KFS::UnlockBlock(uint64_t Block){
        void* BitmapBuffer = mallocK(1); 
        globalPartition->Read(KFSPartitionInfo->bitmapPosition + (Block / 8), 1, BitmapBuffer);
        uint8_t value = *(uint8_t*)((uint64_t)BitmapBuffer);
        WriteBit(value, Block % 8, 0);
        globalPartition->Write(KFSPartitionInfo->bitmapPosition + (Block / 8), 1, BitmapBuffer);
        free(BitmapBuffer);
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
        free(BitmapBuffer);
        return Check;
    }

    void KFS::GetBlockData(uint64_t Block, void* buffer){
        globalPartition->Read(Block * KFSPartitionInfo->BlockSize, KFSPartitionInfo->BlockSize, buffer);
    }

    void KFS::SetBlockData(uint64_t Block, void* buffer){
        globalPartition->Write(Block * KFSPartitionInfo->BlockSize, KFSPartitionInfo->BlockSize, buffer);
    }

    File* KFS::OpenFile(char* filePath){
        FolderInfo* folderInfo = NULL;
        GPT::Partition partitionTemp = GPT::Partition(NULL, NULL);
        GPT::Partition* partition = &partitionTemp;
        void* buffer = globalAllocator.RequestPage();


        char** SplitPath = split(filePath, "://");
        char* PartitionName = SplitPath[0];
        char* Folders = SplitPath[1];
        char** FoldersSlit = split(Folders, "/");
        int count;
        for(count = 0; FoldersSlit[count] != 0; count++); 

        for(int i = 0; i < count; i++){ 
            //find the folder name

            uint64_t byteToScan = folderInfo->firstByte;
            for(int i = 0; i < folderInfo->numberFiles; i++){
                memset(buffer, 0, sizeof(HeaderInfo));
                partition->Read(byteToScan, sizeof(HeaderInfo), buffer); 
                if(!((HeaderInfo*)buffer)->IsFile){
                    memset(buffer, 0, sizeof(FolderInfo));
                    partition->Read(byteToScan + sizeof(HeaderInfo), sizeof(FolderInfo), buffer); 


                    if(((FolderInfo*)buffer)->name == FoldersSlit[i]){ //check the name
                        byteToScan = ((FolderInfo*)buffer)->firstByte;
                    }else{
                        byteToScan = ((FolderInfo*)buffer)->bottomHeader;
                    }

                }else{
                    memset(buffer, 0, sizeof(FileInfo));
                    partition->Read(byteToScan + sizeof(HeaderInfo), sizeof(FileInfo), buffer);

                    //verify if the search is finish 
                    if(((FileInfo*)buffer)->name == FoldersSlit[count]){
                        File* file;
                        file->fileInfo = (FileInfo*)buffer;
                        return file;
                    }
                }                    
            }            
        }        
    }

    void KFS::Close(File* file){

    }
    
    FolderInfo* OpenFolderInFolder(GPT::Partition* Partition, FolderInfo* FolderOpened, char* FolderName){
        void* buffer = globalAllocator.RequestPage();
        uint64_t firstByte = 0;
        uint64_t byteToScan = 0;

        if(FolderOpened == NULL){
            firstByte = Partition->partition->FirstLBA;
        }else{
            firstByte = FolderOpened->firstByte;
        }

        byteToScan = firstByte;

        while(true){            
            memset(buffer, 0, sizeof(HeaderInfo));
            Partition->Read(byteToScan, sizeof(HeaderInfo), buffer); 

            if(((HeaderInfo*)buffer)->IsFile == false){
                memset(buffer, 0, sizeof(FolderInfo));
                Partition->Read(byteToScan + sizeof(HeaderInfo), sizeof(FolderInfo), buffer); 
                if(((FolderInfo*)buffer)->name == FolderName){
                    return (FolderInfo*)buffer;
                }else{
                    byteToScan = ((FolderInfo*)buffer)->bottomHeader;
                }
            }else{
                memset(buffer, 0, sizeof(FileInfo));
                Partition->Read(byteToScan + sizeof(HeaderInfo), sizeof(FileInfo), buffer); 
                byteToScan = ((FileInfo*)buffer)->bottomHeader;
            }

            if(byteToScan == 0){
                return NULL;
            }
        }
    }
}