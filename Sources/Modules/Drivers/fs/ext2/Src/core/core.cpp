#include <core/core.h>

KResult InitializeMount(srv_storage_device_t* StorageDevice){
    mount_info_t* MountInfo = (mount_info_t*)malloc(sizeof(mount_info_t));
    MountInfo->StorageDevice = StorageDevice;

    MountInfo->ReadSuperBlock();

    if(MountInfo->SuperBlock->magic != EXT_SUPER_MAGIC){
        free(MountInfo);
        free(MountInfo->SuperBlock);
        return KFAIL;
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

    ext2_inode_t* InodeTest = MountInfo->FindInodeDirectoryFromPath("home/tests/test1.txt");
    size64_t Size = MountInfo->GetSizeFromInode(InodeTest);
    uintptr_t Buffer = malloc(Size);
    MountInfo->ReadInode(InodeTest, Buffer, NULL, Size);
    uint64_t Block = NULL;
    MountInfo->AllocateBlock(&Block);
    std::printf("%s %x", Buffer, Block);

    return KSUCCESS;
}


KResult mount_info_t::ReadSuperBlock(){
    SuperBlock = (super_block_t*)malloc(sizeof(super_block_t));
    return Srv_ReadDevice(StorageDevice, SuperBlock, EXT_SUPERBLOCK_START, sizeof(super_block_t));
}

KResult mount_info_t::WriteSuperBlock(){
    return Srv_WriteDevice(StorageDevice, SuperBlock, EXT_SUPERBLOCK_START, sizeof(super_block_t));
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

uint64_t mount_info_t::GetBlockGroupFromInode(uint64_t inode){
    return (inode - 1) / SuperBlock->inodes_per_group;
}

uint64_t mount_info_t::GetIndexInodeInsideBlockGroupFromInode(uint64_t inode){
    return (inode - 1) % SuperBlock->inodes_per_group;
}


/* Inode functions */
ext2_inode_t* mount_info_t::GetInode(uint64_t inode){
    ext2_inode_t* Inode = (ext2_inode_t*)calloc(InodeSize);
    ext2_group_descriptor_t* DescriptorGroup = GetDescriptorFromInode(inode);

    uint64_t InodeTableBlock = GetInodeTable(DescriptorGroup);
    uint64_t LocationOfInode = GetLocationFromBlock(InodeTableBlock) + GetIndexInodeInsideBlockGroupFromInode(inode) * InodeSize;
    Srv_ReadDevice(StorageDevice, Inode, LocationOfInode, InodeSize);
    free(DescriptorGroup);
    return Inode;
}

size64_t mount_info_t::GetSizeFromInode(ext2_inode_t* inode){
    uint64_t Size = (uint64_t)inode->size_lo;
    Size |= ((uint64_t)inode->size_hi) << 32;
    return Size;
}


/* Descriptor functions */
ext2_group_descriptor_t* mount_info_t::GetDescriptorFromGroup(uint64_t group){
    ext2_group_descriptor_t* DescriptorGroup = (ext2_group_descriptor_t*)malloc(sizeof(ext2_group_descriptor_t));
    uint64_t DescriptorGroupBlock = GetBlockGroupStartBlock(group) + FirstBlock;
    uint64_t DescriptorGroupOffset = GetLocationFromBlock(DescriptorGroupBlock);

    Srv_ReadDevice(StorageDevice, DescriptorGroup, DescriptorGroupOffset, sizeof(ext2_group_descriptor_t));
    return DescriptorGroup;
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
KResult mount_info_t::ReadInode(ext2_inode_t* inode, uintptr_t buffer, uint64_t start, size64_t size){
    uint64_t ReadLimit = start + size;
    uint64_t ReadLimitBlock = GetNextBlockLocation(ReadLimit);
    
    uint64_t StartBlock = GetBlockFromLocation(start);
    uint64_t NumberOfBlockToRead = ReadLimitBlock - StartBlock;
    uint64_t BufferPosition = (uint64_t)buffer;
    uint64_t SizeToRead = size;
    if(start % BlockSize){
        uint64_t SizeRead = SizeToRead;
        if(SizeRead > BlockSize){
            SizeRead = BlockSize;
        }
        ReadInodeBlock(inode, (uintptr_t)BufferPosition, StartBlock, start % BlockSize, SizeRead - (start % SizeRead));
        BufferPosition += start % BlockSize;
        SizeToRead -= SizeRead;
        NumberOfBlockToRead--;
        StartBlock++;
    }

    for(uint64_t i = 0; i < NumberOfBlockToRead; i++){
        uint64_t SizeRead = SizeToRead;
        if(SizeRead > BlockSize){
            SizeRead = BlockSize;
        }
        ReadInodeBlock(inode, (uintptr_t)BufferPosition, StartBlock + i, NULL, SizeRead);
        BufferPosition += BlockSize;
        SizeToRead -= SizeRead;
    }
    return KSUCCESS;
}

KResult mount_info_t::ReadInodeBlock(ext2_inode_t* inode, uintptr_t buffer, uint64_t block, uint64_t start, size64_t size){
    uint64_t EntryPerBlock = BlockSize / sizeof(uint32_t); 
    if(block < INODE_BLOCK_POINTER_SINGLY_INDIRECT_ACCESS){
        // No redirection
        uint64_t BlockToRead = inode->block[block];
        ReadBlock(buffer, BlockToRead, start, size);
        return KSUCCESS;
    }else{
        block -= INODE_BLOCK_MAX_INDIRECT_BLOCK;
        uint64_t SingleRedirectionLimit = EntryPerBlock;
        if(block < SingleRedirectionLimit){
            // Single redirection
            uint32_t* BlockBuffer = (uint32_t*)malloc(BlockSize);
            uint64_t BlockRedirection0ToRead = inode->block[INODE_BLOCK_POINTER_SINGLY_INDIRECT_ACCESS];
            ReadBlock(BlockBuffer, BlockRedirection0ToRead, NULL, BlockSize);


            uint64_t BlockToRead = BlockBuffer[block];
            ReadBlock(buffer, BlockToRead, start, size);
            return KSUCCESS;
        }else{
            block -= SingleRedirectionLimit;
            uint64_t DoubleRedirectionLimit = exponentInt(EntryPerBlock, 2);

            if(block < DoubleRedirectionLimit){
                // Double redirection
                uint32_t* BlockBuffer = (uint32_t*)malloc(BlockSize);
                uint64_t BlockRedirection0ToRead = inode->block[INODE_BLOCK_POINTER_DOUBLY_INDIRECT_ACCESS];
                ReadBlock(BlockBuffer, BlockRedirection0ToRead, NULL, BlockSize);

                uint64_t BlockRedirection1ToRead = BlockBuffer[block / EntryPerBlock];
                ReadBlock(BlockBuffer, BlockRedirection1ToRead, NULL, BlockSize);


                uint64_t BlockToRead = BlockBuffer[block % EntryPerBlock];
                ReadBlock(buffer, BlockToRead, start, size);
                return KSUCCESS;
            }else{
                block -= DoubleRedirectionLimit;
                uint64_t TripleRedirectionLimit = exponentInt(EntryPerBlock, 3);

                if(block < TripleRedirectionLimit){
                    // Triple redirection
                    uint32_t* BlockBuffer = (uint32_t*)malloc(BlockSize);
                    uint64_t BlockRedirection0ToRead = inode->block[INODE_BLOCK_POINTER_DOUBLY_INDIRECT_ACCESS];
                    ReadBlock(BlockBuffer, BlockRedirection0ToRead, NULL, BlockSize);

                    uint64_t BlockRedirection1ToRead = BlockBuffer[block / DoubleRedirectionLimit];
                    ReadBlock(BlockBuffer, BlockRedirection1ToRead, NULL, BlockSize);

                    uint64_t BlockRedirection2ToRead = BlockBuffer[(block % DoubleRedirectionLimit) / EntryPerBlock];
                    ReadBlock(BlockBuffer, BlockRedirection2ToRead, NULL, BlockSize);

                    uint64_t BlockToRead = BlockBuffer[block % EntryPerBlock];
                    ReadBlock(buffer, BlockToRead, start, size);
                    return KSUCCESS;
                }                 
            }
        } 
    } 
    return KFAIL;
}

KResult mount_info_t::WriteInode(ext2_inode_t* inode, uintptr_t buffer, uint64_t start, size64_t size){
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
        WriteInodeBlock(inode, (uintptr_t)BufferPosition, StartBlock, start % BlockSize, SizeWrite - (start % SizeWrite));
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
        WriteInodeBlock(inode, (uintptr_t)BufferPosition, StartBlock + i, NULL, SizeWrite);
        BufferPosition += BlockSize;
        SizeToWrite -= SizeWrite;
    }
    return KSUCCESS;
}

KResult mount_info_t::WriteInodeBlock(ext2_inode_t* inode, uintptr_t buffer, uint64_t block, uint64_t start, size64_t size){
    uint64_t EntryPerBlock = BlockSize / sizeof(uint32_t); 
    if(block < INODE_BLOCK_POINTER_SINGLY_INDIRECT_ACCESS){
        // No redirection
        uint64_t BlockToWrite = inode->block[block];
        WriteBlock(buffer, BlockToWrite, start, size);
        return KSUCCESS;
    }else{
        block -= INODE_BLOCK_MAX_INDIRECT_BLOCK;
        uint64_t SingleRedirectionLimit = EntryPerBlock;
        if(block < SingleRedirectionLimit){
            // Single redirection
            uint32_t* BlockBuffer = (uint32_t*)malloc(BlockSize);
            uint64_t BlockRedirection0ToRead = inode->block[INODE_BLOCK_POINTER_SINGLY_INDIRECT_ACCESS];
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
                uint64_t BlockRedirection0ToRead = inode->block[INODE_BLOCK_POINTER_DOUBLY_INDIRECT_ACCESS];
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
                    uint64_t BlockRedirection0ToRead = inode->block[INODE_BLOCK_POINTER_DOUBLY_INDIRECT_ACCESS];
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



KResult mount_info_t::ReadBlock(uintptr_t buffer, uint64_t block, uint64_t start, size64_t size){
    uint64_t StartByte = GetLocationFromBlock(block) + start;
    return Srv_ReadDevice(StorageDevice, buffer, StartByte, size);
}

KResult mount_info_t::WriteBlock(uintptr_t buffer, uint64_t block, uint64_t start, size64_t size){
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


ext2_inode_t* mount_info_t::FindInodeDirectoryFromPath(char* path){
    return FindInodeDirectoryFromInodeEntryAndPath(GetInode(EXT_ROOT_INO), path);
}

ext2_inode_t* mount_info_t::FindInodeDirectoryFromInodeEntryAndPath(ext2_inode_t* inode, char* path){
    uint64_t PathEntriesCount;
    char** PathEntries = strsplit(path, "/", &PathEntriesCount);

    ext2_inode_t* InodeIteration = inode;

    for(uint64_t i = 0; i < PathEntriesCount; i++){
        // Find the directory entry in this inode
        ext2_inode_t* InodeFind = FindInodeDirectoryInodeAndEntryFromName(InodeIteration, PathEntries[i]);
        if(InodeFind != NULL){
            free(InodeIteration);
            InodeIteration = InodeFind;
        }else{
            free(InodeIteration);
            return NULL;
        }
    }
    return InodeIteration;
}

ext2_inode_t* mount_info_t::FindInodeDirectoryInodeAndEntryFromName(ext2_inode_t* inode, char* name){
    if(inode->mode & INODE_TYPE_DIRECTORY){
        // Find the directory entry in this inode
        uint64_t Size = GetSizeFromInode(inode);
        uint64_t NumberOfEntries = Size / BlockSize;
        uint64_t NameLenght = strlen(name);
        ext4_directory_entry_t* DirectoryMain = (ext4_directory_entry_t*)malloc(BlockSize);
        for(uint64_t y = 0; y < NumberOfEntries; y++){
            ext4_directory_entry_t* Directory = DirectoryMain;
            ReadInodeBlock(inode, Directory, y, NULL, BlockSize);
            while(Directory->type_indicator != 0 && ((uint64_t)Directory - (uint64_t)DirectoryMain) < BlockSize){
                if(NameLenght == Directory->name_length){
                    if(strncmp(Directory->name, name, Directory->name_length)){
                        free(DirectoryMain);
                        return GetInode(Directory->inode);
                    }
                }
                Directory = (ext4_directory_entry_t*)((uint64_t)Directory + (uint64_t)Directory->size);
            }
        } 
        free(DirectoryMain);        
    }

    return NULL;
}


KResult mount_info_t::AllocateBlock(uint64_t* block){
    if(!SuperBlock->free_blocks_count) return KFAIL;

    uint64_t NumberOfGroup = DivideRoundUp(SuperBlock->blocks_count, SuperBlock->blocks_per_group);

    uint64_t BlockInBitmapBlock = SuperBlock->blocks_per_group;
    uint8_t* BlockBitmap = (uint8_t*)malloc(BlockSize);

    for(uint64_t i = 0; i < NumberOfGroup; i++){
        ext2_group_descriptor_t* Descriptor = GetDescriptorFromGroup(i);
        if(Descriptor->free_blocks_count){
            uint64_t BitmapSize = DivideRoundUp(SuperBlock->blocks_per_group, 8);
            uint64_t BitmapSizeInBlock = DivideRoundUp(BitmapSize, BlockSize);
            for(uint64_t y = 0; y < BitmapSizeInBlock; y++){
                // Load bitmap
                ReadBlock(BlockBitmap, Descriptor->block_bitmap + y, NULL, BlockSize);
                // Search free block
                for(uint64_t z = 0; z < BlockInBitmapBlock; z++){
                    uint64_t TargetByte = z / 8;
                    uint64_t TargetBit = z % 8;
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

                        *block = GetBlockGroupStartBlock(i) + z;
                        free(Descriptor);
                        free(BlockBitmap);
                        return KSUCCESS;
                    }
                }
            }
        }
        free(Descriptor);
    }
    free(BlockBitmap);
    return KFAIL;
}

KResult mount_info_t::FreeBlock(uint64_t block){

}


KResult mount_info_t::AllocateInode(uint64_t* inode){
    if(!SuperBlock->free_inodes_count) return KFAIL;

    uint64_t NumberOfGroup = DivideRoundUp(SuperBlock->inodes_count, SuperBlock->inodes_per_group);

    uint64_t InodeInBitmapBlock = SuperBlock->inodes_per_group;
    uint8_t* BlockBitmap = (uint8_t*)malloc(BlockSize);

    for(uint64_t i = 0; i < NumberOfGroup; i++){
        ext2_group_descriptor_t* Descriptor = GetDescriptorFromGroup(i);
        if(Descriptor->free_inodes_count){
            uint64_t BitmapSize = DivideRoundUp(SuperBlock->inodes_per_group, 8);
            uint64_t BitmapSizeInBlock = DivideRoundUp(BitmapSize, BlockSize);
            for(uint64_t y = 0; y < BitmapSizeInBlock; y++){
                // Load bitmap
                ReadBlock(BlockBitmap, Descriptor->inode_bitmap + y, NULL, BlockSize);
                // Search free block
                for(uint64_t z = 0; z < InodeInBitmapBlock; z++){
                    uint64_t TargetByte = z / 8;
                    uint64_t TargetBit = z % 8;
                    if(!BIT_CHECK(BlockBitmap[TargetByte], TargetBit)){
                        // Update bitmap
                        BIT_SET(BlockBitmap[TargetByte], TargetBit);
                        WriteBlock(&BlockBitmap[TargetByte], Descriptor->block_bitmap + y, TargetByte, 1);

                        // Update descriptor
                        Descriptor->free_inodes_count--;
                        SetDescriptorFromGroup(i, Descriptor);

                        // Update superblock
                        SuperBlock->free_inodes_count--;
                        WriteSuperBlock();

                        *inode = GetBlockGroupStartBlock(i) + z;
                        free(Descriptor);
                        free(BlockBitmap);
                        return KSUCCESS;
                    }
                }
            }
        }
        free(Descriptor);
    }
    free(BlockBitmap);
    return KFAIL;
}

KResult mount_info_t::FreeInode(uint64_t inode){

}
