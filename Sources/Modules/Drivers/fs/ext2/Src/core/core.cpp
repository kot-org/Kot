#include <core/core.h>

/* TODO test driver with : sudo e2fsck -f <partition> */

mount_info_t* InitializeMount(srv_storage_device_t* StorageDevice){
    mount_info_t* MountInfo = (mount_info_t*)malloc(sizeof(mount_info_t));
    MountInfo->StorageDevice = StorageDevice;

    MountInfo->ReadSuperBlock();

    if(MountInfo->SuperBlock->magic != EXT_SUPER_MAGIC){
        free(MountInfo->SuperBlock);
        free(MountInfo);
        return NULL;
    }

    if(MountInfo->SuperBlock->rev_level >= EXT_DYNAMIC_REV){
        super_block_ext2_dynamic_t* SuperBlockDynamic = (super_block_ext2_dynamic_t*)malloc(sizeof(super_block_ext2_dynamic_t));
        Srv_ReadDevice(StorageDevice, SuperBlockDynamic, EXT_SUPERBLOCK_START + sizeof(super_block_t), sizeof(super_block_ext2_dynamic_t));
        MountInfo->FirstInode = SuperBlockDynamic->first_ino;
        MountInfo->InodeSize = SuperBlockDynamic->inode_size;
        MountInfo->RequiredFeature = SuperBlockDynamic->required_features;
        MountInfo->OptionalFeature = SuperBlockDynamic->optional_features;
        free(SuperBlockDynamic);
    }else{
        MountInfo->FirstInode = EXT_GOOD_OLD_FIRST_INO;
        MountInfo->InodeSize = EXT_GOOD_OLD_INODE_SIZE;
        MountInfo->RequiredFeature = NULL;
        MountInfo->OptionalFeature = NULL;
    }

    MountInfo->BlockSize = EXT_LEFT_VALUE_TO_SHIFT_LEFT << MountInfo->SuperBlock->log_block_size;
    MountInfo->FirstBlock = (EXT_SUPERBLOCK_SIZE / MountInfo->BlockSize) + 1;

    MountInfo->ReadGroupDescriptors();

    return MountInfo;
}

uint64_t mount_info_t::GetSpace(){
    return BlockSize * SuperBlock->blocks_count;
}

uint64_t mount_info_t::GetSpaceFree(){
    return BlockSize * SuperBlock->free_blocks_count;
}

KResult mount_info_t::ReadSuperBlock(){
    SuperBlock = (super_block_t*)malloc(sizeof(super_block_t));
    return Srv_ReadDevice(StorageDevice, SuperBlock, EXT_SUPERBLOCK_START, sizeof(super_block_t));
}

KResult mount_info_t::WriteSuperBlock(){
    return Srv_WriteDevice(StorageDevice, SuperBlock, EXT_SUPERBLOCK_START, sizeof(super_block_t));
}

KResult mount_info_t::ReadGroupDescriptors(){
    uint64_t i = (SuperBlock->blocks_count / SuperBlock->blocks_per_group) + ((SuperBlock->blocks_count % SuperBlock->blocks_per_group) ? 1 : 0);
	uint64_t j = (SuperBlock->inodes_count / SuperBlock->inodes_per_group) + ((SuperBlock->inodes_count % SuperBlock->inodes_per_group) ? 1 : 0);
	GroupsCount = (i > j) ? i : j;
    GroupDescriptors = malloc(sizeof(ext2_group_descriptor_t) * GroupsCount);
    uint64_t DescriptorGroupOffset = GetLocationFromBlock(FirstBlock);
    return Srv_ReadDevice(StorageDevice, GroupDescriptors, DescriptorGroupOffset, sizeof(ext2_group_descriptor_t) * GroupsCount);
}

uint64_t mount_info_t::GetLocationFromBlock(uint64_t block){
    return BlockSize * block;
}

uint64_t mount_info_t::GetBlockFromLocation(uint64_t location){
    return location / BlockSize;
}

uint64_t mount_info_t::GetLocationInBlock(uint64_t location){
    return location % BlockSize;
}

uint64_t mount_info_t::GetNextBlockLocation(uint64_t location){
    uint64_t LocationNextBlock = GetBlockFromLocation(location);
    if(GetLocationInBlock(location)){
        LocationNextBlock++;
    }
    return LocationNextBlock;
}

uint64_t mount_info_t::GetBlockGroupStartBlock(uint64_t group){
    return SuperBlock->blocks_per_group * group;
}

uint64_t mount_info_t::GetBlockGroupFromBlock(uint64_t block){
    return block / SuperBlock->blocks_per_group;
}

uint64_t mount_info_t::GetBlockPositionInsideBlockGroupFromBlock(uint64_t block){
    return block % SuperBlock->blocks_per_group;
}

uint64_t mount_info_t::GetBlockGroupFromInode(uint64_t inode){
    return (inode - 1) / SuperBlock->inodes_per_group;
}

uint64_t mount_info_t::GetIndexInodeInsideBlockGroupFromInode(uint64_t inode){
    return (inode - 1) % SuperBlock->inodes_per_group;
}


/* Inode functions */
inode_t* mount_info_t::GetInode(uint64_t position){
    inode_t* Inode = (inode_t*)calloc(InodeSize + INODE_EXTRA_SIZE);
    ext2_group_descriptor_t* DescriptorGroup = GetDescriptorFromInode(position);

    uint64_t InodeTableBlock = GetInodeTable(DescriptorGroup);
    uint64_t LocationOfInode = GetLocationFromBlock(InodeTableBlock) + GetIndexInodeInsideBlockGroupFromInode(position) * InodeSize;
    Inode->InodeLocation = LocationOfInode;

    Srv_ReadDevice(StorageDevice, &Inode->Inode, LocationOfInode, InodeSize);
    return Inode;
}

KResult mount_info_t::SetInode(inode_t* inode){
    Srv_WriteDevice(StorageDevice, &inode->Inode, inode->InodeLocation, InodeSize);
    return KSUCCESS;
}

size64_t mount_info_t::GetSizeFromInode(inode_t* inode){
    uint64_t Size = (uint64_t)inode->Inode.size_lo;
    Size |= ((uint64_t)inode->Inode.size_hi) << 32;
    return Size;
}

KResult mount_info_t::SetSizeFromInode(inode_t* inode, size64_t size){
    inode->Inode.size_lo = (uint32_t)(size & 0xffffffff);
    inode->Inode.size_hi = (uint32_t)(size >> 32);
    return KSUCCESS;
}

uint64_t mount_info_t::GetDirectoryIndicatorFromInode(inode_t* inode){
    switch (inode->Inode.mode & 0xF000){
        case INODE_TYPE_FIFO:
            return DIRECTORY_TYPE_FIFO;

        case INODE_TYPE_CHARACTER_DEVICE:
            return DIRECTORY_TYPE_CHARACTER_DEVICE;

        case INODE_TYPE_DIRECTORY:
            return DIRECTORY_TYPE_DIRECTORY;

        case INODE_TYPE_BLOCK_DEVICE:
            return DIRECTORY_TYPE_BLOCK_DEVICE;

        case INODE_TYPE_REGULAR_FILE:
            return DIRECTORY_TYPE_REGULAR_FILE;

        case INODE_TYPE_SYMBOLIC_LINK:
            return DIRECTORY_TYPE_SYMBOLIC_LINK;

        case INODE_TYPE_UNIX_SOCKET:
            return DIRECTORY_TYPE_UNIX_SOCKET;

        default:
            return NULL;
    }
}


/* Descriptor functions */
ext2_group_descriptor_t* mount_info_t::GetDescriptorFromGroup(uint64_t group){
    return (ext2_group_descriptor_t*)((uint64_t)GroupDescriptors + sizeof(ext2_group_descriptor_t) * group);
}

ext2_group_descriptor_t* mount_info_t::GetDescriptorFromBlock(uint64_t block){
    return GetDescriptorFromGroup(GetBlockGroupFromBlock(block)); 
}

ext2_group_descriptor_t* mount_info_t::GetDescriptorFromInode(uint64_t inode){
    return GetDescriptorFromGroup(GetBlockGroupFromInode(inode)); 
}

KResult mount_info_t::SetDescriptorFromGroup(uint64_t group, ext2_group_descriptor_t* descriptor){
    uint64_t DescriptorGroupBlock = GetBlockGroupStartBlock(group) + FirstBlock;
    uint64_t DescriptorGroupOffset = GetLocationFromBlock(DescriptorGroupBlock);

    Srv_WriteDevice(StorageDevice, descriptor, DescriptorGroupOffset, sizeof(ext2_group_descriptor_t));
    return KSUCCESS;
}

uint64_t mount_info_t::GetBlockBitmap(ext2_group_descriptor_t* descriptor){
    uint64_t BlockBitmap = (uint64_t)descriptor->block_bitmap;
    return BlockBitmap;
}

uint64_t mount_info_t::GetInodeBitmap(ext2_group_descriptor_t* descriptor){
    uint64_t InodeBitmap = (uint64_t)descriptor->inode_bitmap;
    return InodeBitmap;
}

uint64_t mount_info_t::GetInodeTable(ext2_group_descriptor_t* descriptor){
    uint64_t InodeTable = (uint64_t)descriptor->inode_table;
    return InodeTable;
}

uint64_t mount_info_t::GetFreeBlocksCount(ext2_group_descriptor_t* descriptor){
    uint64_t FreeBlocksCount = (uint64_t)descriptor->free_blocks_count;
    return FreeBlocksCount;
}

uint64_t mount_info_t::GetFreeInodesCount(ext2_group_descriptor_t* descriptor){
    uint64_t FreeInodesCount = (uint64_t)descriptor->free_inodes_count;
    return FreeInodesCount;
}

uint64_t mount_info_t::GetUsedDirCount(ext2_group_descriptor_t* descriptor){
    uint64_t UsedDirCount = (uint64_t)descriptor->used_dirs_count;
    return UsedDirCount;
}


/* Inode function */
KResult mount_info_t::ReadInode(inode_t* inode, kot_key_mem_t* key, uint64_t start, size64_t size){
    uint64_t ReadLimit = start + size;
    uint64_t ReadLimitBlock = GetNextBlockLocation(ReadLimit);
    
    uint64_t StartBlock = GetBlockFromLocation(start);
    uint64_t NumberOfBlockToRead = ReadLimitBlock - StartBlock;
    uint64_t BufferPosition = 0;
    uint64_t SizeToRead = size;

    // Cache system
    uint64_t redirection0 = 0;
    uint64_t redirection1 = 0;
    uint64_t redirection2 = 0;
    uint32_t* redirectioncache0 = NULL;
    uint32_t* redirectioncache1 = NULL;
    uint32_t* redirectioncache2 = NULL;

    // Allocate multiple request struct
    srv_storage_multiple_requests_t* MultipleRequest = (srv_storage_multiple_requests_t*)malloc(sizeof(srv_storage_multiple_requests_t) + NumberOfBlockToRead * sizeof(srv_storage_request_t));
    MultipleRequest->RequestsCount = NumberOfBlockToRead;
    MultipleRequest->TotalSize = SizeToRead;
    MultipleRequest->IsWrite = false;
    MultipleRequest->MemoryKey = NULL;

    uint64_t RequestCount = 0;

    if(start % BlockSize){
        uint64_t SizeRead = SizeToRead;
        if(SizeRead > BlockSize){
            SizeRead = BlockSize;
        }
        int64_t BlockToRead = GetInodeBlock(inode, StartBlock, &redirection0, &redirection1, &redirection2, &redirectioncache0, &redirectioncache1, &redirectioncache2);
        uint64_t PositionToRead = GetLocationFromBlock(BlockToRead) + start % BlockSize;

        MultipleRequest->Requests[RequestCount].Start = PositionToRead;
        MultipleRequest->Requests[RequestCount].Size = SizeRead - (start % SizeRead);
        MultipleRequest->Requests[RequestCount].BufferOffset = BufferPosition;

        BufferPosition += start % BlockSize;
        SizeToRead -= SizeRead;
        NumberOfBlockToRead--;
        StartBlock++;
        RequestCount++;
    }

    for(uint64_t i = 0; i < NumberOfBlockToRead; i++){
        uint64_t SizeRead = SizeToRead;
        if(SizeRead > BlockSize){
            SizeRead = BlockSize;
        }
        int64_t BlockToRead = GetInodeBlock(inode, StartBlock + i, &redirection0, &redirection1, &redirection2, &redirectioncache0, &redirectioncache1, &redirectioncache2);
        uint64_t PositionToRead = GetLocationFromBlock(BlockToRead);

        MultipleRequest->Requests[RequestCount].Start = PositionToRead;
        MultipleRequest->Requests[RequestCount].Size = SizeRead;
        MultipleRequest->Requests[RequestCount].BufferOffset = BufferPosition;

        BufferPosition += BlockSize;
        SizeToRead -= SizeRead;
        RequestCount++;
    }

    free(redirectioncache0);
    free(redirectioncache1);
    free(redirectioncache2);

    struct srv_storage_device_callback_t* Callback = Srv_SendMultipleRequests(StorageDevice, MultipleRequest);
    KResult Status = Callback->Status;
    *key = (kot_key_mem_t)Callback->Data;
    free(Callback);

    return Status;
}
KResult mount_info_t::ReadInode(inode_t* inode, void* buffer, uint64_t start, size64_t size){
    kot_key_mem_t Key;
    KResult Status = ReadInode(inode, &Key, start, size);
    if(Status == KSUCCESS){
        Sys_AcceptMemoryField(Sys_GetProcess(), Key, &buffer);
    }
    return Status;
}

KResult mount_info_t::ReadInodeBlock(struct inode_t* inode, void* buffer, uint64_t block, uint64_t start, size64_t size){
    uint64_t redirection0 = 0;
    uint64_t redirection1 = 0;
    uint64_t redirection2 = 0;
    uint32_t* redirectioncache0 = NULL;
    uint32_t* redirectioncache1 = NULL;
    uint32_t* redirectioncache2 = NULL;
    int64_t BlockToRead = GetInodeBlock(inode, block, &redirection0, &redirection1, &redirection2, &redirectioncache0, &redirectioncache1, &redirectioncache2);
    free(redirectioncache0);
    free(redirectioncache1);
    free(redirectioncache2);
    if(BlockToRead == -1) return KFAIL;
    return ReadBlock(buffer, BlockToRead, start, size);
}

int64_t mount_info_t::GetInodeBlock(inode_t* inode, uint64_t block, uint64_t* redirection0, uint64_t* redirection1, uint64_t* redirection2, uint32_t** redirectioncache0, uint32_t** redirectioncache1, uint32_t** redirectioncache2){
    uint64_t EntryPerBlock = BlockSize / sizeof(uint32_t); 
    if(block < INODE_BLOCK_POINTER_SINGLY_INDIRECT_ACCESS){
        // No redirection
        return inode->Inode.block[block];
    }else{
        block -= INODE_BLOCK_MAX_INDIRECT_BLOCK + 1;
        uint64_t SingleRedirectionLimit = EntryPerBlock;
        if(block < SingleRedirectionLimit){
            // Single redirection
            if(*redirectioncache0 == NULL || *redirection0 != INODE_BLOCK_POINTER_SINGLY_INDIRECT_ACCESS){
                if(*redirectioncache0 != NULL) free(*redirectioncache0);
                *redirectioncache0 = (uint32_t*)malloc(BlockSize);
                *redirection0 = INODE_BLOCK_POINTER_SINGLY_INDIRECT_ACCESS;
                
                uint64_t BlockRedirection0ToRead = inode->Inode.block[INODE_BLOCK_POINTER_SINGLY_INDIRECT_ACCESS];
                ReadBlock(*redirectioncache0, BlockRedirection0ToRead, NULL, BlockSize);
            }

            return(*redirectioncache0)[block];
        }else{
            block -= SingleRedirectionLimit;
            uint64_t DoubleRedirectionLimit = exponentInt(EntryPerBlock, 2);

            if(block < DoubleRedirectionLimit){
                // Double redirection
                if(*redirectioncache0 == NULL || *redirection0 != INODE_BLOCK_POINTER_DOUBLY_INDIRECT_ACCESS){
                    if(*redirectioncache0 != NULL) free(*redirectioncache0);
                    *redirectioncache0 = (uint32_t*)malloc(BlockSize);
                    *redirection0 = INODE_BLOCK_POINTER_DOUBLY_INDIRECT_ACCESS;
                    uint64_t BlockRedirection0ToRead = inode->Inode.block[INODE_BLOCK_POINTER_DOUBLY_INDIRECT_ACCESS];
                    ReadBlock(*redirectioncache0, BlockRedirection0ToRead, NULL, BlockSize);
                }

                if(*redirectioncache1 == NULL || *redirection1 != block / EntryPerBlock){
                    if(*redirectioncache1 != NULL) free(*redirectioncache1);
                    *redirectioncache1 = (uint32_t*)malloc(BlockSize);
                    *redirection1 = block / EntryPerBlock;
                    uint64_t BlockRedirection1ToRead = (*redirectioncache0)[block / EntryPerBlock];
                    ReadBlock(*redirectioncache1, BlockRedirection1ToRead, NULL, BlockSize);
                }


                return (*redirectioncache1)[block % EntryPerBlock];
            }else{
                block -= DoubleRedirectionLimit;
                uint64_t TripleRedirectionLimit = exponentInt(EntryPerBlock, 3);

                if(block < TripleRedirectionLimit){
                    // Triple redirection
                    if(*redirectioncache0 == NULL || *redirection0 != INODE_BLOCK_POINTER_TRIPLY_INDIRECT_ACCESS){
                        if(*redirectioncache0 != NULL) free(*redirectioncache0);
                        *redirectioncache0 = (uint32_t*)malloc(BlockSize);
                        *redirection0 = INODE_BLOCK_POINTER_TRIPLY_INDIRECT_ACCESS;
                        uint64_t BlockRedirection0ToRead = inode->Inode.block[INODE_BLOCK_POINTER_TRIPLY_INDIRECT_ACCESS];
                        ReadBlock(*redirectioncache0, BlockRedirection0ToRead, NULL, BlockSize);
                    }

                    if(*redirectioncache1 == NULL || *redirection1 != block / DoubleRedirectionLimit){
                        if(*redirectioncache1 != NULL) free(*redirectioncache1);
                        *redirectioncache1 = (uint32_t*)malloc(BlockSize);
                        *redirection1 = block / DoubleRedirectionLimit;
                        uint64_t BlockRedirection1ToRead = (*redirectioncache0)[block / DoubleRedirectionLimit];
                        ReadBlock(*redirectioncache1, BlockRedirection1ToRead, NULL, BlockSize);
                    }

                    if(*redirectioncache2 == NULL || *redirection2 != (block % DoubleRedirectionLimit) / EntryPerBlock){
                        if(*redirectioncache2 != NULL) free(*redirectioncache2);
                        *redirectioncache2 = (uint32_t*)malloc(BlockSize);
                        *redirection2 = (block % DoubleRedirectionLimit) / EntryPerBlock;
                        uint64_t BlockRedirection2ToRead = (*redirectioncache1)[(block % DoubleRedirectionLimit) / EntryPerBlock];
                        ReadBlock(*redirectioncache1, BlockRedirection2ToRead, NULL, BlockSize);
                    }

                    return (*redirectioncache2)[block % EntryPerBlock];
                }                 
            }
        } 
    }
    return -1;
}

KResult mount_info_t::WriteInode(inode_t* inode, void* buffer, uint64_t start, size64_t size, bool is_data_end){
    // Allocate or free if necessary
    atomicAcquire(&Lock, 0);
    uint64_t SizeRequested = start + size;
    uint64_t InodeSize = GetSizeFromInode(inode);
    uint64_t SizeRequestedInBlock = DIV_ROUND_UP(SizeRequested, BlockSize);
    uint64_t SizeOfInodeInBlock = DIV_ROUND_UP(InodeSize, BlockSize);
    if(SizeRequestedInBlock > SizeOfInodeInBlock){
        // Allocate
        uint64_t BlockToAllocate = SizeRequestedInBlock - SizeOfInodeInBlock;
        KResult status = AllocateInodeBlocks(inode, SizeOfInodeInBlock, BlockToAllocate);
        if(status != KSUCCESS){
            atomicUnlock(&Lock, 0);
            return status;
        }
    }else if(is_data_end && SizeRequestedInBlock < SizeOfInodeInBlock){
        // Free
        uint64_t BlockToFree = SizeRequestedInBlock - SizeOfInodeInBlock;
        KResult status = FreeInodeBlocks(inode, SizeOfInodeInBlock, BlockToFree);
        if(status != KSUCCESS){
            atomicUnlock(&Lock, 0);
            return status;
        } 
    }

    if(is_data_end || SizeRequested > InodeSize){
        SetSizeFromInode(inode, SizeRequested);
        SetInode(inode);
    }

    atomicUnlock(&Lock, 0);
    
    uint64_t WriteLimit = start + size;
    uint64_t WriteLimitBlock = GetNextBlockLocation(WriteLimit);
    
    uint64_t StartBlock = GetBlockFromLocation(start);
    uint64_t NumberOfBlockToWrite = WriteLimitBlock - StartBlock;
    uint64_t BufferPosition = (uint64_t)buffer;
    uint64_t SizeToWrite = size;
    if(start % BlockSize){
        uint64_t SizeWrite = SizeToWrite;
        if(SizeWrite > BlockSize){
            SizeWrite = BlockSize;
        }
        WriteInodeBlock(inode, (void*)BufferPosition, StartBlock, start % BlockSize, SizeWrite - (start % SizeWrite));
        BufferPosition += start % BlockSize;
        SizeToWrite -= SizeWrite;
        NumberOfBlockToWrite--;
        StartBlock++;
    }

    for(uint64_t i = 0; i < NumberOfBlockToWrite; i++){
        uint64_t SizeWrite = SizeToWrite;
        if(SizeWrite > BlockSize){
            SizeWrite = BlockSize;
        }
        WriteInodeBlock(inode, (void*)BufferPosition, StartBlock + i, NULL, SizeWrite);
        BufferPosition += BlockSize;
        SizeToWrite -= SizeWrite;
    }
    return KSUCCESS;
}

KResult mount_info_t::WriteInodeBlock(inode_t* inode, void* buffer, uint64_t block, uint64_t start, size64_t size){
    uint64_t EntryPerBlock = BlockSize / sizeof(uint32_t); 
    if(block < INODE_BLOCK_POINTER_SINGLY_INDIRECT_ACCESS){
        // No redirection
        uint64_t BlockToWrite = inode->Inode.block[block];
        WriteBlock(buffer, BlockToWrite, start, size);
        return KSUCCESS;
    }else{
        block -= INODE_BLOCK_MAX_INDIRECT_BLOCK;
        uint64_t SingleRedirectionLimit = EntryPerBlock;
        if(block < SingleRedirectionLimit){
            // Single redirection
            uint32_t* BlockBuffer = (uint32_t*)malloc(BlockSize);
            uint64_t BlockRedirection0ToRead = inode->Inode.block[INODE_BLOCK_POINTER_SINGLY_INDIRECT_ACCESS];
            ReadBlock(BlockBuffer, BlockRedirection0ToRead, NULL, BlockSize);


            uint64_t BlockToWrite = BlockBuffer[block];
            WriteBlock(buffer, BlockToWrite, start, size);
            return KSUCCESS;
        }else{
            block -= SingleRedirectionLimit;
            uint64_t DoubleRedirectionLimit = exponentInt(EntryPerBlock, 2);

            if(block < DoubleRedirectionLimit){
                // Double redirection
                uint32_t* BlockBuffer = (uint32_t*)malloc(BlockSize);
                uint64_t BlockRedirection0ToRead = inode->Inode.block[INODE_BLOCK_POINTER_DOUBLY_INDIRECT_ACCESS];
                ReadBlock(BlockBuffer, BlockRedirection0ToRead, NULL, BlockSize);

                uint64_t BlockRedirection1ToRead = BlockBuffer[block / EntryPerBlock];
                ReadBlock(BlockBuffer, BlockRedirection1ToRead, NULL, BlockSize);


                uint64_t BlockToWrite = BlockBuffer[block % EntryPerBlock];
                WriteBlock(buffer, BlockToWrite, start, size);
                return KSUCCESS;
            }else{
                block -= DoubleRedirectionLimit;
                uint64_t TripleRedirectionLimit = exponentInt(EntryPerBlock, 3);

                if(block < TripleRedirectionLimit){
                    // Triple redirection
                    uint32_t* BlockBuffer = (uint32_t*)malloc(BlockSize);
                    uint64_t BlockRedirection0ToRead = inode->Inode.block[INODE_BLOCK_POINTER_TRIPLY_INDIRECT_ACCESS];
                    ReadBlock(BlockBuffer, BlockRedirection0ToRead, NULL, BlockSize);

                    uint64_t BlockRedirection1ToRead = BlockBuffer[block / DoubleRedirectionLimit];
                    ReadBlock(BlockBuffer, BlockRedirection1ToRead, NULL, BlockSize);

                    uint64_t BlockRedirection2ToRead = BlockBuffer[(block % DoubleRedirectionLimit) / EntryPerBlock];
                    ReadBlock(BlockBuffer, BlockRedirection2ToRead, NULL, BlockSize);

                    uint64_t BlockToWrite = BlockBuffer[block % EntryPerBlock];
                    WriteBlock(buffer, BlockToWrite, start, size);
                    return KSUCCESS;
                }                 
            }
        } 
    } 
    return KFAIL;
}



KResult mount_info_t::ReadBlock(void* buffer, uint64_t block, uint64_t start, size64_t size){
    uint64_t StartByte = GetLocationFromBlock(block) + start;
    return Srv_ReadDevice(StorageDevice, buffer, StartByte, size);
}

KResult mount_info_t::WriteBlock(void* buffer, uint64_t block, uint64_t start, size64_t size){
    uint64_t StartByte = GetLocationFromBlock(block) + start;
    return Srv_WriteDevice(StorageDevice, buffer, StartByte, size);
}


uint64_t mount_info_t::GetLocationFromInode(uint64_t inode){
    ext2_group_descriptor_t* DescriptorGroup = (ext2_group_descriptor_t*)malloc(sizeof(ext2_group_descriptor_t));
    uint64_t DescriptorGroupBlock = GetBlockGroupStartBlock(GetBlockGroupFromInode(inode)) + SuperBlock->first_data_block;
    uint64_t DescriptorGroupOffset = GetLocationFromBlock(DescriptorGroupBlock);

    Srv_ReadDevice(StorageDevice, DescriptorGroup, DescriptorGroupOffset, sizeof(ext2_group_descriptor_t));

    uint64_t InodeTableBlock = (uint64_t)DescriptorGroup->inode_table;
    uint64_t LocationOfInode = DescriptorGroupOffset + GetIndexInodeInsideBlockGroupFromInode(inode) * InodeSize;
    return LocationOfInode;
}


inode_t* mount_info_t::FindInodeFromPath(char* path){
    return FindInodeFromInodeEntryAndPath(GetInode(EXT_ROOT_INO), path);
}

inode_t* mount_info_t::FindInodeFromInodeEntryAndPath(inode_t* inode, char* path){
    inode_t* InodeIteration = inode;
    if(path != NULL){
        uint64_t PathEntriesCount;
        char** PathEntries = strsplit(path, "/", &PathEntriesCount);


        for(uint64_t i = 0; i < PathEntriesCount; i++){
            // Find the directory entry in this inode
            if(!(InodeIteration->Inode.mode & INODE_TYPE_DIRECTORY)){
                free(InodeIteration);
                return NULL;
            }
            inode_t* InodeFind = FindInodeInodeAndEntryFromName(InodeIteration, PathEntries[i]);
            if(InodeFind != NULL){
                if(InodeIteration != inode){
                    free(InodeIteration);
                }
                InodeIteration = InodeFind;
            }else{
                if(InodeIteration != inode){
                    free(InodeIteration);
                }
                return NULL;
            }
        }
    }
    return InodeIteration;
}

inode_t* mount_info_t::FindInodeInodeAndEntryFromName(inode_t* inode, char* name){
    // Find the directory entry in this inode
    uint64_t Size = GetSizeFromInode(inode);
    uint64_t NumberOfEntries = DIV_ROUND_UP(Size, BlockSize);
    uint64_t NameLenght = strlen(name);
    ext2_directory_entry_t* DirectoryMain = (ext2_directory_entry_t*)malloc(BlockSize);
    for(uint64_t y = 0; y < NumberOfEntries; y++){
        ext2_directory_entry_t* Directory = DirectoryMain;
        ReadInodeBlock(inode, Directory, y, NULL, BlockSize);
        while(Directory->type_indicator != 0 && ((uint64_t)Directory - (uint64_t)DirectoryMain) < BlockSize){
            if(NameLenght == Directory->name_length){
                if(strncmp(Directory->name, name, Directory->name_length)){
                    uint32_t inode = Directory->inode;
                    free(DirectoryMain);
                    return GetInode(inode);
                }
            }
            Directory = (ext2_directory_entry_t*)((uint64_t)Directory + (uint64_t)Directory->size);
        }
    } 
    free(DirectoryMain);     

    return NULL;
}

KResult mount_info_t::LinkInodeToDirectory(inode_t* directory_inode, inode_t* inode, char* name){
    directory_inode->Inode.atime = GetPosixTime();
    SetInode(directory_inode);
    // Find the directory entry in this inode
    uint64_t NameLenght = strlen(name);
    uint64_t SizeRequestByTheEntry = sizeof(ext2_directory_entry_t) + NameLenght;
    if(SizeRequestByTheEntry % 4){
        SizeRequestByTheEntry -= SizeRequestByTheEntry % 4;
        SizeRequestByTheEntry += 4;
    }

    uint64_t Size = GetSizeFromInode(directory_inode);
    uint64_t NumberOfEntries = DIV_ROUND_UP(Size, BlockSize);
    ext2_directory_entry_t* DirectoryMain = (ext2_directory_entry_t*)malloc(BlockSize);
    ext2_directory_entry_t* Directory;
    for(uint64_t y = 0; y < NumberOfEntries; y++){
        Directory = DirectoryMain;
        ReadInodeBlock(directory_inode, DirectoryMain, y, NULL, BlockSize);
        while(((uint64_t)Directory - (uint64_t)DirectoryMain) < BlockSize){
            ext2_directory_entry_t* Entry = (ext2_directory_entry_t*)((uint64_t)Directory + (uint64_t)Directory->size);
            if(Entry->type_indicator != 0){
                Directory = (ext2_directory_entry_t*)((uint64_t)Directory + (uint64_t)Directory->size);
            }else{
                break;
            }
        }
    }

    if((uint64_t)Directory - (uint64_t)DirectoryMain >= SizeRequestByTheEntry){
        uint64_t OldSize = Directory->size;
        Directory->size = sizeof(ext2_directory_entry_t) + Directory->name_length;
        if(Directory->size % 4){
            Directory->size -= Directory->size % 4;
            Directory->size += 4;
        }

        Directory = (ext2_directory_entry_t*)((uint64_t)Directory + (uint64_t)Directory->size);
        Directory->inode = inode->InodeLocation;
        Directory->size = OldSize - SizeRequestByTheEntry;
        Directory->name_length = NameLenght;
        Directory->type_indicator = GetDirectoryIndicatorFromInode(inode);
        memcpy(&Directory->name, name, NameLenght);

        ext2_directory_entry_t* NextNullEntry = (ext2_directory_entry_t*)((uint64_t)Directory + (uint64_t)SizeRequestByTheEntry);
        NextNullEntry->type_indicator = 0;
        
        WriteInodeBlock(directory_inode, DirectoryMain, NumberOfEntries - 1, NULL, BlockSize);

        Directory = DirectoryMain;
        while(Directory->type_indicator != 0 && ((uint64_t)Directory - (uint64_t)DirectoryMain) < BlockSize){
            Directory = (ext2_directory_entry_t*)((uint64_t)Directory + (uint64_t)Directory->size);
        }
    }else{
        AllocateInodeBlock(directory_inode, NumberOfEntries);

        uint64_t OldSize = Directory->size;
        if(Directory->size % 4){
            Directory->size -= Directory->size % 4;
            Directory->size += 4;
        }

        Directory = (ext2_directory_entry_t*)((uint64_t)Directory + (uint64_t)Directory->size);
        Directory->inode = inode->InodeLocation;
        Directory->size = OldSize - SizeRequestByTheEntry;
        Directory->name_length = NameLenght;
        Directory->type_indicator = GetDirectoryIndicatorFromInode(inode);
        memcpy(Directory->name, name, NameLenght);

        ext2_directory_entry_t* NextNullEntry = (ext2_directory_entry_t*)((uint64_t)Directory + (uint64_t)SizeRequestByTheEntry);
        NextNullEntry->type_indicator = 0;

        WriteInodeBlock(directory_inode, Directory, NumberOfEntries, NULL, BlockSize);

        SetSizeFromInode(directory_inode, Size + BlockSize);
        SetInode(directory_inode);
    }


    free(DirectoryMain);     

    return KSUCCESS;    
}

KResult mount_info_t::UnlinkInodeToDirectory(inode_t* inode, char* name){
    // Find the directory entry in this inode
    uint64_t NameLenght = strlen(name);
    uint64_t SizeRequestByTheEntry = sizeof(ext2_directory_entry_t) + NameLenght;
    uint64_t Size = GetSizeFromInode(inode);
    void* Buffer = malloc(Size);
    ext2_directory_entry_t* EntryToDelete = NULL;
    ext2_directory_entry_t* Directory = (ext2_directory_entry_t*)Buffer;
    ReadInode(inode, Directory, NULL, Size);

    while(Directory->type_indicator != 0){
        if(NameLenght == Directory->name_length){
            if(strncmp(Directory->name, name, Directory->name_length)){
                EntryToDelete = Directory;
            }
        }
        Directory = (ext2_directory_entry_t*)((uint64_t)Directory + (uint64_t)Directory->size);
    }

    if(!EntryToDelete){
        free(Buffer);
        return KFAIL;
    }

    ext2_directory_entry_t*  EntryToDeleteEnd = (ext2_directory_entry_t* )((uint64_t)EntryToDelete + EntryToDelete->size);
    uint64_t NextSize = Size - (uint64_t)EntryToDeleteEnd - (uint64_t)Buffer;
    memcpy(EntryToDelete, EntryToDeleteEnd, NextSize);

    WriteInode(inode, Buffer, NULL, Size, true);
     
    free(Buffer);     

    return KSUCCESS;
}


KResult mount_info_t::AllocateBlock(uint64_t* block){
    if(!SuperBlock->free_blocks_count) return KFAIL;

    uint64_t NumberOfGroup = DIV_ROUND_UP(SuperBlock->blocks_count, SuperBlock->blocks_per_group);

    uint64_t BlockInBitmapBlock = BlockSize * 8;
    uint8_t* BlockBitmap = (uint8_t*)malloc(BlockSize);

    atomicAcquire(&Lock, 1);
    for(uint64_t i = 0; i < NumberOfGroup; i++){
        ext2_group_descriptor_t* Descriptor = GetDescriptorFromGroup(i);
        if(Descriptor->free_blocks_count){
            uint64_t BitmapSize = DIV_ROUND_UP(SuperBlock->blocks_per_group, 8);
            uint64_t BitmapSizeInBlock = DIV_ROUND_UP(BitmapSize, BlockSize);
            for(uint64_t y = 0; y < BitmapSizeInBlock; y++){
                // Load bitmap
                ReadBlock(BlockBitmap, Descriptor->block_bitmap + y, NULL, BlockSize);
                // Search free block
                for(uint64_t z = 0; z < BlockInBitmapBlock; z++){
                    uint64_t Position = z + y * BlockInBitmapBlock;
                    uint64_t TargetByte = Position / 8;
                    uint64_t TargetBit = Position % 8;
                    if(!BIT_CHECK(BlockBitmap[TargetByte], TargetBit)){
                        // Update bitmap
                        BIT_SET(BlockBitmap[TargetByte], TargetBit);
                        WriteBlock(&BlockBitmap[TargetByte], Descriptor->block_bitmap + y, TargetByte, 1);

                        // Update descriptor
                        Descriptor->free_blocks_count--;
                        SetDescriptorFromGroup(i, Descriptor);

                        // Update superblock
                        SuperBlock->free_blocks_count--;
                        WriteSuperBlock();

                        *block = GetBlockGroupStartBlock(i) + Position;
                        free(BlockBitmap);
                        atomicUnlock(&Lock, 1);
                        return KSUCCESS;
                    }
                }
            }
        }
    }
    atomicUnlock(&Lock, 1);
    free(BlockBitmap);
    return KFAIL;
}

KResult mount_info_t::FreeBlock(uint64_t block){
    uint8_t* BitmapBuffer = (uint8_t*)malloc(1);

    uint64_t Group = GetBlockGroupFromBlock(block);
    ext2_group_descriptor_t* Descriptor = GetDescriptorFromGroup(Group);

    uint64_t Position = GetBlockPositionInsideBlockGroupFromBlock(block);
    uint64_t TargetByte = Position / 8;
    uint64_t TargetBit = Position % 8;
    uint64_t BlockInBitmapBlock = BlockSize * 8;
    uint64_t BlockBitmapPosition = Descriptor->block_bitmap + (Position / BlockInBitmapBlock);

    ReadBlock(BitmapBuffer, BlockBitmapPosition, TargetByte, 1);

    atomicAcquire(&Lock, 1);
    if(BIT_CHECK(*BitmapBuffer, TargetBit)){
        // Update bitmap
        BIT_CLEAR(*BitmapBuffer, TargetBit);
        WriteBlock(BitmapBuffer, BlockBitmapPosition, TargetByte, 1);

        // Update descriptor
        Descriptor->free_blocks_count++;
        SetDescriptorFromGroup(Group, Descriptor);

        // Update superblock
        SuperBlock->free_blocks_count++;
        WriteSuperBlock();

        free(BitmapBuffer);

        atomicUnlock(&Lock, 1);
        return KSUCCESS;
    }
    atomicUnlock(&Lock, 1);
    free(BitmapBuffer);

    return KFAIL;
}


KResult mount_info_t::AllocateInode(uint64_t* inode){
    if(!SuperBlock->free_inodes_count) return KFAIL;

    uint64_t NumberOfGroup = DIV_ROUND_UP(SuperBlock->inodes_count, SuperBlock->inodes_per_group);

    uint64_t InodeInBitmapBlock = BlockSize * 8;
    uint8_t* BlockBitmap = (uint8_t*)malloc(BlockSize);


    for (uint64_t i = 0; i < NumberOfGroup; i++)
    {
        ext2_group_descriptor_t* Descriptor = GetDescriptorFromGroup(i);
        if (Descriptor->free_inodes_count)
        {
            uint64_t BitmapSize = DIV_ROUND_UP(SuperBlock->inodes_per_group, 8);
            uint64_t BitmapSizeInBlock = DIV_ROUND_UP(BitmapSize, BlockSize);
            for (uint64_t y = 0; y < BitmapSizeInBlock; y++)
            {
                // Load bitmap
                ReadBlock(BlockBitmap, Descriptor->inode_bitmap + y, NULL, BlockSize);
                // Search free block
                for (uint64_t z = 0; z < BlockSize * 8; z++)
                {
                    uint64_t TargetByte = z / 8;
                    uint64_t TargetBit = z % 8;
                    if (!BIT_CHECK(BlockBitmap[TargetByte], TargetBit))
                    {
                        // Update bitmap
                        BIT_SET(BlockBitmap[TargetByte], TargetBit);
                        Srv_WriteDevice(StorageDevice, &BlockBitmap[TargetByte], Descriptor->inode_bitmap + y + TargetByte, 1);

                        // Update descriptor
                        Descriptor->free_inodes_count--;
                        SetDescriptorFromGroup(i, Descriptor);

                        // Update superblock
                        SuperBlock->free_inodes_count--;
                        WriteSuperBlock();

                        *inode = GetBlockGroupStartBlock(i) + z;
                        free(BlockBitmap);
                        return KSUCCESS;
                    }
                }
            }
        }
    }
    free(BlockBitmap);
    return KFAIL;
}

KResult mount_info_t::FreeInode(uint64_t inode){
    uint8_t* BitmapBuffer = (uint8_t*)malloc(1);

    uint64_t Group = GetBlockGroupFromInode(inode);
    ext2_group_descriptor_t* Descriptor = GetDescriptorFromGroup(Group);

    uint64_t Position = GetIndexInodeInsideBlockGroupFromInode(inode);
    uint64_t TargetByte = Position / 8;
    uint64_t TargetBit = Position % 8;
    uint64_t InodeInBitmapBlock = BlockSize * 8;
    uint64_t BlockBitmapPosition = Descriptor->inode_bitmap + (Position / InodeInBitmapBlock);

    ReadBlock(BitmapBuffer, BlockBitmapPosition, TargetByte, 1);

    if(BIT_CHECK(*BitmapBuffer, TargetBit)){
        // Update bitmap
        BIT_CLEAR(*BitmapBuffer, TargetBit);
        WriteBlock(BitmapBuffer, BlockBitmapPosition, TargetByte, 1);

        // Update descriptor
        Descriptor->free_inodes_count++;
        SetDescriptorFromGroup(Group, Descriptor);

        // Update superblock
        SuperBlock->free_inodes_count++;
        WriteSuperBlock();

        free(BitmapBuffer);

        return KSUCCESS;
    }
    free(BitmapBuffer);

    return KFAIL;
}

KResult mount_info_t::DeleteInode(struct inode_t* inode){
    FreeInodeBlocks(inode, NULL, DIV_ROUND_UP(GetSizeFromInode(inode), BlockSize));
    SetSizeFromInode(inode, NULL);
    
    inode->Inode.dtime = GetPosixTime();

    SetInode(inode);

    FreeInode(inode->InodeLocation);

    return KSUCCESS;
}


KResult mount_info_t::AllocateInodeBlocks(inode_t* inode, uint64_t block, size64_t block_size){
    for(uint64_t i = 0; i < block_size; i++){
        AllocateInodeBlock(inode, block + i);
    }
    return KSUCCESS;
}

KResult mount_info_t::FreeInodeBlocks(inode_t* inode, uint64_t block, size64_t block_size){
    for(uint64_t i = 0; i < block_size; i++){
        FreeInodeBlock(inode, block + i);
    }
    return KSUCCESS;
}

KResult mount_info_t::AllocateInodeBlock(inode_t* inode, uint64_t block){
    uint64_t EntryPerBlock = BlockSize / sizeof(uint32_t); 
    if(block < INODE_BLOCK_POINTER_SINGLY_INDIRECT_ACCESS){
        // No redirection
        if(inode->Inode.block[block] == NULL){
            AllocateBlock((uint64_t*)&inode->Inode.block[block]);
            SetInode(inode);
        }
        return KSUCCESS;
    }else{
        block -= INODE_BLOCK_MAX_INDIRECT_BLOCK;
        uint64_t SingleRedirectionLimit = EntryPerBlock;
        if(block < SingleRedirectionLimit){
            // Single redirection
            uint32_t* BlockBuffer = (uint32_t*)malloc(BlockSize);
            uint64_t BlockRedirection0ToRead = inode->Inode.block[INODE_BLOCK_POINTER_SINGLY_INDIRECT_ACCESS];

            if(inode->Inode.block[INODE_BLOCK_POINTER_SINGLY_INDIRECT_ACCESS] == NULL){
                AllocateBlock((uint64_t*)&inode->Inode.block[INODE_BLOCK_POINTER_SINGLY_INDIRECT_ACCESS]);
                BlockRedirection0ToRead = inode->Inode.block[INODE_BLOCK_POINTER_SINGLY_INDIRECT_ACCESS];
                SetInode(inode);
                memset(BlockBuffer, NULL, BlockSize);
            }else{
                ReadBlock(BlockBuffer, BlockRedirection0ToRead, NULL, BlockSize);
            }
            if(BlockBuffer[block] == NULL){
                AllocateBlock((uint64_t*)&BlockBuffer[block]);
                WriteBlock(BlockBuffer, BlockRedirection0ToRead, NULL, BlockSize);
            }
            return KSUCCESS;
        }else{
            block -= SingleRedirectionLimit;
            uint64_t DoubleRedirectionLimit = exponentInt(EntryPerBlock, 2);

            if(block < DoubleRedirectionLimit){
                // Double redirection
                uint32_t* BlockBuffer = (uint32_t*)malloc(BlockSize);
                uint64_t BlockRedirection0ToRead = inode->Inode.block[INODE_BLOCK_POINTER_DOUBLY_INDIRECT_ACCESS];

                if(inode->Inode.block[INODE_BLOCK_POINTER_DOUBLY_INDIRECT_ACCESS] == NULL){
                    AllocateBlock((uint64_t*)&inode->Inode.block[INODE_BLOCK_POINTER_DOUBLY_INDIRECT_ACCESS]);
                    BlockRedirection0ToRead = inode->Inode.block[INODE_BLOCK_POINTER_DOUBLY_INDIRECT_ACCESS];
                    SetInode(inode);
                    memset(BlockBuffer, NULL, BlockSize);
                }else{
                    ReadBlock(BlockBuffer, BlockRedirection0ToRead, NULL, BlockSize);
                }

                uint64_t BlockRedirection1ToRead = BlockBuffer[block / EntryPerBlock];

                if(BlockBuffer[block / EntryPerBlock] == NULL){
                    AllocateBlock((uint64_t*)&BlockBuffer[block / EntryPerBlock]);
                    BlockRedirection1ToRead = BlockBuffer[block / EntryPerBlock];
                    WriteBlock(BlockBuffer, BlockRedirection0ToRead, NULL, BlockSize);
                    memset(BlockBuffer, NULL, BlockSize);
                }else{
                    ReadBlock(BlockBuffer, BlockRedirection1ToRead, NULL, BlockSize);
                }

                if(BlockBuffer[block % EntryPerBlock] == NULL){
                    AllocateBlock((uint64_t*)&BlockBuffer[block % EntryPerBlock]);
                    WriteBlock(BlockBuffer, BlockRedirection1ToRead, NULL, BlockSize);
                }
                return KSUCCESS;
            }else{
                block -= DoubleRedirectionLimit;
                uint64_t TripleRedirectionLimit = exponentInt(EntryPerBlock, 3);

                if(block < TripleRedirectionLimit){
                    // Triple redirection
                    uint32_t* BlockBuffer = (uint32_t*)malloc(BlockSize);
                    uint64_t BlockRedirection0ToRead = inode->Inode.block[INODE_BLOCK_POINTER_TRIPLY_INDIRECT_ACCESS];

                    if(BlockRedirection0ToRead == NULL){
                        AllocateBlock((uint64_t*)&inode->Inode.block[INODE_BLOCK_POINTER_TRIPLY_INDIRECT_ACCESS]);
                        BlockRedirection0ToRead = inode->Inode.block[INODE_BLOCK_POINTER_TRIPLY_INDIRECT_ACCESS];
                        SetInode(inode);
                        memset(BlockBuffer, NULL, BlockSize);
                    }else{
                        ReadBlock(BlockBuffer, BlockRedirection0ToRead, NULL, BlockSize);
                    }

                    uint64_t BlockRedirection1ToRead = BlockBuffer[block / DoubleRedirectionLimit];

                    if(BlockBuffer[block / DoubleRedirectionLimit] == NULL){
                        AllocateBlock((uint64_t*)&BlockBuffer[block / DoubleRedirectionLimit]);
                        BlockRedirection1ToRead = BlockBuffer[block / DoubleRedirectionLimit];
                        WriteBlock(BlockBuffer, BlockRedirection0ToRead, NULL, BlockSize);
                        memset(BlockBuffer, NULL, BlockSize);
                    }else{
                        ReadBlock(BlockBuffer, BlockRedirection1ToRead, NULL, BlockSize);
                    }

                    uint64_t BlockRedirection2ToRead = BlockBuffer[(block % DoubleRedirectionLimit) / EntryPerBlock];

                    if(BlockBuffer[(block % DoubleRedirectionLimit) / EntryPerBlock] == NULL){
                        AllocateBlock((uint64_t*)&BlockBuffer[(block % DoubleRedirectionLimit) / EntryPerBlock]);
                        BlockRedirection2ToRead = BlockBuffer[(block % DoubleRedirectionLimit) / EntryPerBlock];
                        WriteBlock(BlockBuffer, BlockRedirection1ToRead, NULL, BlockSize);
                        memset(BlockBuffer, NULL, BlockSize);
                    }else{
                        ReadBlock(BlockBuffer, BlockRedirection2ToRead, NULL, BlockSize);
                    }

                    if(BlockBuffer[block % EntryPerBlock] == NULL){
                        AllocateBlock((uint64_t*)&BlockBuffer[block % EntryPerBlock]);
                        WriteBlock(BlockBuffer, BlockRedirection2ToRead, NULL, BlockSize);
                    }
                    return KSUCCESS;
                }                 
            }
        } 
    }
    return KFAIL;    
}

KResult mount_info_t::FreeInodeBlock(inode_t* inode, uint64_t block){
    uint64_t EntryPerBlock = BlockSize / sizeof(uint32_t); 
    if(block < INODE_BLOCK_POINTER_SINGLY_INDIRECT_ACCESS){
        // No redirection
        if(inode->Inode.block[block] != NULL){
            FreeBlock(inode->Inode.block[block]);
            inode->Inode.block[block] = NULL;
            SetInode(inode);
        }
        return KSUCCESS;
    }else{
        block -= INODE_BLOCK_MAX_INDIRECT_BLOCK;
        uint64_t SingleRedirectionLimit = EntryPerBlock;
        if(block < SingleRedirectionLimit){
            // Single redirection
            uint32_t* BlockBuffer = (uint32_t*)malloc(BlockSize);
            uint64_t BlockRedirection0ToRead = inode->Inode.block[INODE_BLOCK_POINTER_SINGLY_INDIRECT_ACCESS];

            if(BlockRedirection0ToRead == NULL) return KFAIL;

            if(block == NULL){
                FreeBlock(BlockRedirection0ToRead);
                inode->Inode.block[block] = NULL;
                SetInode(inode);
            }

            ReadBlock(BlockBuffer, BlockRedirection0ToRead, NULL, BlockSize);

            if(BlockBuffer[block] != NULL){
                FreeBlock(BlockBuffer[block]);
                BlockBuffer[block] = NULL;
                WriteBlock(BlockBuffer, BlockRedirection0ToRead, NULL, BlockSize);
            }
            return KSUCCESS;
        }else{
            block -= SingleRedirectionLimit;
            uint64_t DoubleRedirectionLimit = exponentInt(EntryPerBlock, 2);

            if(block < DoubleRedirectionLimit){
                // Double redirection
                uint32_t* BlockBuffer = (uint32_t*)malloc(BlockSize);
                uint64_t BlockRedirection0ToRead = inode->Inode.block[INODE_BLOCK_POINTER_DOUBLY_INDIRECT_ACCESS];

                if(BlockRedirection0ToRead == NULL) return KFAIL;

                if(block == NULL){
                    FreeBlock(BlockRedirection0ToRead);
                    inode->Inode.block[INODE_BLOCK_POINTER_DOUBLY_INDIRECT_ACCESS] = NULL;
                    SetInode(inode);
                }

                ReadBlock(BlockBuffer, BlockRedirection0ToRead, NULL, BlockSize);

                uint64_t BlockRedirection1ToRead = BlockBuffer[block / EntryPerBlock];

                if(BlockRedirection1ToRead == NULL) return KFAIL;

                if(block / EntryPerBlock == NULL){
                    FreeBlock(BlockBuffer[block / EntryPerBlock]);
                    BlockBuffer[block / EntryPerBlock] == NULL;
                    WriteBlock(BlockBuffer, BlockRedirection0ToRead, NULL, BlockSize);
                }

                ReadBlock(BlockBuffer, BlockRedirection1ToRead, NULL, BlockSize);


                if(BlockBuffer[block % EntryPerBlock] != NULL){
                    FreeBlock(BlockBuffer[block % EntryPerBlock]);
                    BlockBuffer[block % EntryPerBlock] = NULL;
                    WriteBlock(BlockBuffer, BlockRedirection1ToRead, NULL, BlockSize);
                }
                return KSUCCESS;
            }else{
                block -= DoubleRedirectionLimit;
                uint64_t TripleRedirectionLimit = exponentInt(EntryPerBlock, 3);

                if(block < TripleRedirectionLimit){
                    // Triple redirection
                    uint32_t* BlockBuffer = (uint32_t*)malloc(BlockSize);
                    uint64_t BlockRedirection0ToRead = inode->Inode.block[INODE_BLOCK_POINTER_TRIPLY_INDIRECT_ACCESS];

                    if(BlockRedirection0ToRead == NULL) return KFAIL;


                    if(block == NULL){
                        FreeBlock(inode->Inode.block[INODE_BLOCK_POINTER_TRIPLY_INDIRECT_ACCESS]);
                        inode->Inode.block[INODE_BLOCK_POINTER_TRIPLY_INDIRECT_ACCESS] = NULL;
                        SetInode(inode);
                    }

                    ReadBlock(BlockBuffer, BlockRedirection0ToRead, NULL, BlockSize);

                    uint64_t BlockRedirection1ToRead = BlockBuffer[block / DoubleRedirectionLimit];
                    if(BlockRedirection1ToRead == NULL) return KFAIL;

                    if(block / DoubleRedirectionLimit == NULL){
                        AllocateBlock((uint64_t*)&BlockBuffer[block / DoubleRedirectionLimit]);
                        WriteBlock(BlockBuffer, BlockRedirection0ToRead, NULL, BlockSize);
                        memset(BlockBuffer, NULL, BlockSize);
                    }else{
                        ReadBlock(BlockBuffer, BlockRedirection1ToRead, NULL, BlockSize);
                    }

                    uint64_t BlockRedirection2ToRead = BlockBuffer[(block % DoubleRedirectionLimit) / EntryPerBlock];

                    if(BlockRedirection2ToRead == NULL) return KFAIL;

                    if(BlockBuffer[(block % DoubleRedirectionLimit) / EntryPerBlock] == NULL){
                        AllocateBlock((uint64_t*)&BlockBuffer[(block % DoubleRedirectionLimit) / EntryPerBlock]);
                        WriteBlock(BlockBuffer, BlockRedirection1ToRead, NULL, BlockSize);
                        memset(BlockBuffer, NULL, BlockSize);
                    }else{
                        ReadBlock(BlockBuffer, BlockRedirection2ToRead, NULL, BlockSize);
                    }

                    uint64_t BlockToRead = BlockBuffer[block % EntryPerBlock];

                    if(BlockBuffer[block % EntryPerBlock] != NULL){
                        AllocateBlock((uint64_t*)&BlockBuffer[block % EntryPerBlock]);
                        WriteBlock(BlockBuffer, BlockRedirection2ToRead, NULL, BlockSize);
                    }
                    return KSUCCESS;
                }                 
            }
        } 
    }
    return KFAIL;
}

KResult mount_info_t::CheckPermissions(inode_t* inode, kot_permissions_t permissions, kot_permissions_t permissions_requested){
    // TODO
    return KSUCCESS;
}


KResult mount_info_t::CreateDir(char* path, mode_t mode, kot_permissions_t permissions){
    // TODO : mode
    std::StringBuilder* CreatePathSB = new std::StringBuilder(path);
    char* CreatePathDirectory = CreatePathSB->substr(NULL, CreatePathSB->indexOf("/", 0, true));
    char* CreateName = CreatePathSB->substr(CreatePathSB->indexOf("/", 0, true), CreatePathSB->length());

    inode_t* Directory = FindInodeFromPath(path);
    if(CheckPermissions(Directory, permissions, Storage_Permissions_Write) != KSUCCESS){
        free(Directory);
        return KFAIL;
    }
    
    uint64_t DirInode = NULL;
    KResult Status = AllocateInode(&DirInode);
    if(Status != KSUCCESS) return Status;

    inode_t* Inode = GetInode(DirInode);
    if(Inode == NULL) return KFAIL;

    memset(&Inode->Inode, 0, sizeof(ext2_inode_t));

    Inode->Inode.ctime = GetPosixTime();
    Inode->Inode.links_count = 1;
    Inode->Inode.mode = INODE_TYPE_DIRECTORY;
    Inode->Inode.mode |= 0xfff;

    SetSizeFromInode(Inode, NULL);
    SetInode(Inode);

    LinkInodeToDirectory(Directory, Inode, CreateName);

    free(CreatePathSB);
    free(CreatePathDirectory);
    free(CreateName);
    free(Directory);
    free(Inode);
    return KSUCCESS;
}

KResult mount_info_t::RemoveDir(char* path, kot_permissions_t permissions){
    inode_t* Inode = FindInodeFromPath(path);
    if(Inode->Inode.mode != INODE_TYPE_DIRECTORY){
        free(Inode);
        return KFAIL;
    }
    std::StringBuilder* OldPathSB = new std::StringBuilder(path);
    char* OldPathDirectory = OldPathSB->substr(NULL, OldPathSB->indexOf("/", 0, true));
    char* OldName = OldPathSB->substr(OldPathSB->indexOf("/", 0, true), OldPathSB->length());

    inode_t* DirectoryOld = FindInodeFromPath(OldPathDirectory);

    if(CheckPermissions(DirectoryOld, permissions, Storage_Permissions_Write) != KSUCCESS){
        free(DirectoryOld);
        free(Inode);
        return KSUCCESS; 
    }

    UnlinkInodeToDirectory(DirectoryOld, OldName);
    DeleteInode(Inode);

    free(DirectoryOld);
    free(Inode);
    return KSUCCESS;  
}

ext_directory_t* mount_info_t::OpenDir(char* path, kot_permissions_t permissions){
    inode_t* RootInode = GetInode(EXT_ROOT_INO);
    ext_directory_t* Target = OpenDir(GetInode(EXT_ROOT_INO), path, permissions);
    free(RootInode);
    return Target;
}

ext_directory_t* mount_info_t::OpenDir(inode_t* inode, char* path, kot_permissions_t permissions){
    inode_t* Target = FindInodeFromInodeEntryAndPath(inode, path);
    if(Target){
        if(Target->Inode.mode & INODE_TYPE_DIRECTORY && CheckPermissions(Target, permissions, Storage_Permissions_Read) == KSUCCESS){
            ext_directory_t* Directory = (ext_directory_t*)malloc(sizeof(ext_directory_t));
            Directory->Inode = Target;
            Directory->MountInfo = this;
            Directory->Permissions = permissions;
            return Directory;
        }

        free(Target);
    }
    return NULL;
}

read_dir_data* ext_directory_t::ReadDir(uint64_t index){
    Inode->Inode.atime = GetPosixTime();
    MountInfo->SetInode(Inode);

    uint64_t Size = MountInfo->GetSizeFromInode(Inode);
    uint64_t NumberOfEntries = Size / MountInfo->BlockSize;
    ext2_directory_entry_t* DirectoryMain = (ext2_directory_entry_t*)malloc(MountInfo->BlockSize);
    uint64_t IndexIterator = 0;
    for(uint64_t y = 0; y < NumberOfEntries; y++){
        ext2_directory_entry_t* Directory = DirectoryMain;
        MountInfo->ReadInodeBlock(Inode, Directory, y, NULL, MountInfo->BlockSize);
        while(Directory->type_indicator != 0 && ((uint64_t)Directory - (uint64_t)DirectoryMain) < MountInfo->BlockSize){
            if(Directory->type_indicator == DIRECTORY_TYPE_REGULAR_FILE || Directory->type_indicator == DIRECTORY_TYPE_DIRECTORY){
                if(index == IndexIterator){
                    // Target reach
                    read_dir_data* ReadDirData = (read_dir_data*)malloc(sizeof(read_dir_data));
                    ReadDirData->Name = (char*)malloc(Directory->name_length + 1);
                    memcpy(ReadDirData->Name, Directory->name, Directory->name_length);
                    ReadDirData->NameLength = Directory->name_length;
                    ReadDirData->Name[Directory->name_length] = NULL;
                    ReadDirData->IsFile = Directory->type_indicator == DIRECTORY_TYPE_REGULAR_FILE;
                    free(DirectoryMain);
                    return ReadDirData;
                }
                IndexIterator++;
            }
            Directory = (ext2_directory_entry_t*)((uint64_t)Directory + (uint64_t)Directory->size);
        }
    }
    free(DirectoryMain); 
    return NULL;
}

KResult ext_directory_t::CloseDir(){
    free(this);
    return KSUCCESS;
}

size64_t ext_directory_t::GetDirCount(){
    uint64_t DirCount = 0;
    uint64_t Size = MountInfo->GetSizeFromInode(Inode);
    uint64_t NumberOfEntries = Size / MountInfo->BlockSize;
    ext2_directory_entry_t* DirectoryMain = (ext2_directory_entry_t*)malloc(MountInfo->BlockSize);
    uint64_t IndexIterator = 0;
    for(uint64_t y = 0; y < NumberOfEntries; y++){
        ext2_directory_entry_t* Directory = DirectoryMain;
        MountInfo->ReadInodeBlock(Inode, Directory, y, NULL, MountInfo->BlockSize);
        while(Directory->type_indicator != 0 && ((uint64_t)Directory - (uint64_t)DirectoryMain) < MountInfo->BlockSize){
            DirCount++;
            Directory = (ext2_directory_entry_t*)((uint64_t)Directory + (uint64_t)Directory->size);
        }
    }
    
    free(DirectoryMain);

    return DirCount;
}


KResult mount_info_t::Rename(char* old_path, char* new_path, kot_permissions_t permissions){
    inode_t* Inode = FindInodeFromPath(old_path);
    if(Inode->Inode.mode != INODE_TYPE_DIRECTORY && !(Inode->Inode.mode & INODE_TYPE_REGULAR_FILE)){
        free(Inode);
        return KFAIL;
    }

    std::StringBuilder* OldPathSB = new std::StringBuilder(old_path);
    char* OldPathDirectory = OldPathSB->substr(NULL, OldPathSB->indexOf("/", 0, true));
    char* OldName = OldPathSB->substr(OldPathSB->indexOf("/", 0, true), OldPathSB->length());

    std::StringBuilder* NewPathSB = new std::StringBuilder(old_path);
    char* NewPathDirectory = NewPathSB->substr(NULL, NewPathSB->indexOf("/", 0, true));
    char* NewName = OldPathSB->substr(OldPathSB->indexOf("/", 0, true), OldPathSB->length());

    inode_t* DirectoryOld = FindInodeFromPath(OldPathDirectory);
    if(CheckPermissions(DirectoryOld, permissions, Storage_Permissions_Write) != KSUCCESS){
        free(Inode);
        free(OldPathDirectory);
        free(OldName);
        free(NewPathDirectory);
        free(NewName);
        free(OldPathSB);
        free(DirectoryOld);
        return KFAIL;
    } 

    inode_t* DirectoryNew = FindInodeFromPath(NewPathDirectory);
    if(CheckPermissions(DirectoryNew, permissions, Storage_Permissions_Write) != KSUCCESS){
        free(Inode);
        free(OldPathDirectory);
        free(OldName);
        free(NewPathDirectory);
        free(NewName);
        free(OldPathSB);
        free(DirectoryOld);
        free(DirectoryNew);
        return KFAIL;
    } 

    UnlinkInodeToDirectory(DirectoryOld, OldName);
    LinkInodeToDirectory(DirectoryNew, Inode, NewName);
    
    free(Inode);
    free(OldPathDirectory);
    free(OldName);
    free(NewPathDirectory);
    free(NewName);
    free(OldPathSB);
    free(DirectoryOld);
    free(DirectoryNew);
    return KSUCCESS;
}

KResult mount_info_t::CreateFile(char* path, char* name, kot_permissions_t permissions){
    inode_t* Directory = FindInodeFromPath(path);
    if(CheckPermissions(Directory, permissions, Storage_Permissions_Create) != KSUCCESS){
        free(Directory);
        return KFAIL;
    }

    uint64_t FileInode = NULL;
    KResult Status = AllocateInode(&FileInode);
    if(Status != KSUCCESS) return Status;

    inode_t* Inode = GetInode(FileInode);
    if(Inode == NULL) return KFAIL;

    memset(&Inode->Inode, 0, sizeof(ext2_inode_t));

    Inode->Inode.ctime = GetPosixTime();
    Inode->Inode.links_count = 1;
    Inode->Inode.mode = INODE_TYPE_REGULAR_FILE;
    Inode->Inode.mode |= 0xfff;

    SetSizeFromInode(Inode, NULL);
    SetInode(Inode);

    LinkInodeToDirectory(Directory, Inode, name);

    free(Directory);
    free(Inode);
    return KSUCCESS;
}

KResult mount_info_t::RemoveFile(char* path, kot_permissions_t permissions){
    inode_t* Inode = FindInodeFromPath(path);

    if(!Inode){
       return KFAIL; 
    } 

    if(!(Inode->Inode.mode & INODE_TYPE_REGULAR_FILE) || CheckPermissions(Inode, permissions, Storage_Permissions_Write) != KSUCCESS){
        free(Inode);
        return KNOTALLOW;
    }

    std::StringBuilder* OldPathSB = new std::StringBuilder(path);
    inode_t* Directory;
    char* OldName;
    int64_t OldPathDirectoryIndex = OldPathSB->indexOf("/", 0, true);

    if(OldPathDirectoryIndex != -1){
        char* OldPathDirectory = OldPathSB->substr(NULL, OldPathDirectoryIndex);
        char* OldName = OldPathSB->substr(OldPathDirectoryIndex, OldPathSB->length());
        Directory = FindInodeFromPath(OldPathDirectory);
        free(OldPathDirectory);
    }else{
        Directory = GetInode(EXT_ROOT_INO);
        OldName = OldPathSB->substr(NULL, OldPathSB->length());
    }
    free(OldPathSB);


    if(CheckPermissions(Directory, permissions, Storage_Permissions_Write) != KSUCCESS){
        free(Directory);
        free(OldName);
        free(Inode);
        return KFAIL;
    }

    assert(UnlinkInodeToDirectory(Directory, OldName) == KSUCCESS);
    assert(DeleteInode(Inode) == KSUCCESS);

    free(Directory);
    free(OldName);
    free(Inode);
    return KSUCCESS; 
}

ext_file_t* mount_info_t::OpenFile(char* path, kot_permissions_t permissions){
    inode_t* RootInode = GetInode(EXT_ROOT_INO);
    ext_file_t* Target = OpenFile(RootInode, path, permissions);
    free(RootInode);
    return Target;
}

ext_file_t* mount_info_t::OpenFile(inode_t* inode, char* path, kot_permissions_t permissions){
    inode_t* Target = FindInodeFromInodeEntryAndPath(inode, path);
    if(!Target){
        if(permissions & Storage_Permissions_Create){
            std::StringBuilder* CreatePathSB = new std::StringBuilder(path);
            char* CreatePathDirectory = CreatePathSB->substr(NULL, CreatePathSB->indexOf("/", 0, true));
            char* CreateName = CreatePathSB->substr(CreatePathSB->indexOf("/", 0, true), CreatePathSB->length());
            CreateFile(CreatePathDirectory, CreateName, permissions);
            Target = FindInodeFromInodeEntryAndPath(inode, path);
            free(CreatePathSB);
            free(CreatePathDirectory);
            free(CreateName);
            if(!Target) return NULL;
        }else{
            return NULL;
        }
    }

    if(Target->Inode.mode & INODE_TYPE_REGULAR_FILE && CheckPermissions(Target, permissions, Storage_Permissions_Read) == KSUCCESS){
        ext_file_t* File = (ext_file_t*)malloc(sizeof(ext_file_t));
        File->Inode = Target;
        File->MountInfo = this;
        File->Size = GetSizeFromInode(File->Inode);
        File->Permissions = permissions;
        return File;
    }

    free(Target);
    return NULL;
}

KResult ext_file_t::ReadFile(void* buffer, uint64_t start, size64_t size){
    if(Permissions & Storage_Permissions_Read){
        Inode->Inode.atime = GetPosixTime();
        MountInfo->SetInode(Inode);
        return MountInfo->ReadInode(Inode, buffer, start, size);
    }
    return KFAIL;
}

KResult ext_file_t::ReadFile(kot_key_mem_t* key, uint64_t start, size64_t size){
    if(Permissions & Storage_Permissions_Read){
        Inode->Inode.atime = GetPosixTime();
        MountInfo->SetInode(Inode);
        return MountInfo->ReadInode(Inode, key, start, size);
    }
    return KFAIL;
}

KResult ext_file_t::WriteFile(void* buffer, uint64_t start, size64_t size, bool is_data_end){
    if(Permissions & Storage_Permissions_Write){
        Inode->Inode.mtime = GetPosixTime();
        MountInfo->SetInode(Inode);
        return MountInfo->WriteInode(Inode, buffer, start, size, is_data_end);
    }
    return KFAIL;
}

size64_t ext_file_t::GetSize(){
    return MountInfo->GetSizeFromInode(Inode);
}

KResult ext_file_t::CloseFile(){
    free(this);
    return KSUCCESS;
}