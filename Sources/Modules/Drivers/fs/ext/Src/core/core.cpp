#include <core/core.h>

KResult InitializeMount(srv_storage_device_t* StorageDevice){
    mount_info_t* MountInfo = (mount_info_t*)malloc(sizeof(mount_info_t));
    MountInfo->SuperBlock = (super_block_t*)malloc(sizeof(super_block_t));
    Srv_ReadDevice(StorageDevice, MountInfo->SuperBlock, EXT_SUPERBLOCK_START, sizeof(super_block_t));

    if(MountInfo->SuperBlock->magic != EXT_SUPER_MAGIC){
        free(MountInfo);
        free(MountInfo->SuperBlock);
        return KFAIL;
    }

    if(MountInfo->SuperBlock->rev_level >= EXT_DYNAMIC_REV){
        super_block_ext4_dynamic_t* SuperBlockDynamic = (super_block_ext4_dynamic_t*)malloc(sizeof(super_block_ext4_dynamic_t));
        Srv_ReadDevice(StorageDevice, SuperBlockDynamic, EXT_SUPERBLOCK_START + sizeof(super_block_t), sizeof(super_block_ext4_dynamic_t));
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

    MountInfo->StorageDevice = StorageDevice;

    MountInfo->BlockSize = EXT_LEFT_VALUE_TO_SHIFT_LEFT << MountInfo->SuperBlock->log_block_size;
    MountInfo->FirstBlock = (EXT_SUPERBLOCK_SIZE / MountInfo->BlockSize) + 1;

    for(uint64_t i = EXT_ROOT_INO; i < EXT_ROOT_INO + 1; i++){
        ext4_inode_t* Inode = MountInfo->GetInode(i);
        uint64_t Size = MountInfo->GetSizeFromInode(Inode);
        uint64_t EntryNum = Size / MountInfo->BlockSize;
        for(uint8_t i = 0; i < MountInfo->BlockSize; i++){
            ext4_directory_entry_t* Directory = (ext4_directory_entry_t*)malloc(MountInfo->BlockSize);
            MountInfo->ReadInodeBlock(Inode, Directory, i, NULL, MountInfo->BlockSize);
            while(Directory->type_indicator != 0){
                Directory = (ext4_directory_entry_t*)((uint64_t)Directory + (uint64_t)Directory->size);
                std::printf("%s", Directory->name);
            }
        }
    }
    return KSUCCESS;
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
ext4_inode_t* mount_info_t::GetInode(uint64_t inode){
    ext4_inode_t* Inode = (ext4_inode_t*)malloc(sizeof(ext4_inode_t));
    ext4_group_descriptor_t* DescriptorGroup = GetDescriptorFromInode(inode);

    uint64_t InodeTableBlock = GetInodeTable(DescriptorGroup);
    uint64_t LocationOfInode = GetLocationFromBlock(InodeTableBlock) + GetIndexInodeInsideBlockGroupFromInode(inode) * InodeSize;
    Srv_ReadDevice(StorageDevice, Inode, LocationOfInode, sizeof(ext4_inode_t));
    return Inode;
}

uint64_t mount_info_t::GetSizeFromInode(ext4_inode_t* inode){
    uint64_t Size = (uint64_t)inode->size_lo;
    if(RequiredFeature & EXT4_FEATURE_REQUIRED_64BIT){
        Size |= ((uint64_t)inode->size_hi << 32);
    }
    return Size;
}


/* Descriptor functions */
ext4_group_descriptor_t* mount_info_t::GetDescriptorFromInode(uint64_t inode){
    ext4_group_descriptor_t* DescriptorGroup = (ext4_group_descriptor_t*)malloc(sizeof(ext4_group_descriptor_t));
    uint64_t DescriptorGroupBlock = GetBlockGroupStartBlock(GetBlockGroupFromInode(inode)) + FirstBlock;
    uint64_t DescriptorGroupOffset = GetLocationFromBlock(DescriptorGroupBlock);

    Srv_ReadDevice(StorageDevice, DescriptorGroup, DescriptorGroupOffset, sizeof(ext4_group_descriptor_t));
    return DescriptorGroup;
}

uint64_t mount_info_t::GetBlockBitmap(ext4_group_descriptor_t* descriptor){
    uint64_t BlockBitmap = (uint64_t)descriptor->block_bitmap_lo;
    if(RequiredFeature & EXT4_FEATURE_REQUIRED_64BIT){
        BlockBitmap |= ((uint64_t)descriptor->block_bitmap_hi << 32);
    }
    return BlockBitmap;
}

uint64_t mount_info_t::GetInodeBitmap(ext4_group_descriptor_t* descriptor){
    uint64_t InodeBitmap = (uint64_t)descriptor->inode_bitmap_lo;
    if(RequiredFeature & EXT4_FEATURE_REQUIRED_64BIT){
        InodeBitmap |= ((uint64_t)descriptor->inode_bitmap_hi << 32);
    }
    return InodeBitmap;
}

uint64_t mount_info_t::GetInodeTable(ext4_group_descriptor_t* descriptor){
    uint64_t InodeTable = (uint64_t)descriptor->inode_table_lo;
    if(RequiredFeature & EXT4_FEATURE_REQUIRED_64BIT){
        InodeTable |= ((uint64_t)descriptor->inode_table_hi << 32);
    }
    return InodeTable;
}

uint64_t mount_info_t::GetFreeBlocksCount(ext4_group_descriptor_t* descriptor){
    uint64_t FreeBlocksCount = (uint64_t)descriptor->free_blocks_count_lo;
    if(RequiredFeature & EXT4_FEATURE_REQUIRED_64BIT){
        FreeBlocksCount |= ((uint64_t)descriptor->free_blocks_count_hi << 32);
    }
    return FreeBlocksCount;
}

uint64_t mount_info_t::GetFreeInodesCount(ext4_group_descriptor_t* descriptor){
    uint64_t FreeInodesCount = (uint64_t)descriptor->free_inodes_count_lo;
    if(RequiredFeature & EXT4_FEATURE_REQUIRED_64BIT){
        FreeInodesCount |= ((uint64_t)descriptor->free_inodes_count_hi << 32);
    }
    return FreeInodesCount;
}

uint64_t mount_info_t::GetUsedDirCount(ext4_group_descriptor_t* descriptor){
    uint64_t UsedDirCount = (uint64_t)descriptor->used_dirs_count_lo;
    if(RequiredFeature & EXT4_FEATURE_REQUIRED_64BIT){
        UsedDirCount |= ((uint64_t)descriptor->used_dirs_count_hi << 32);
    }
    return UsedDirCount;
}

uint64_t mount_info_t::GetExcludeBitmap(ext4_group_descriptor_t* descriptor){
    uint64_t ExcludeBitmap = (uint64_t)descriptor->exclude_bitmap_lo;
    if(RequiredFeature & EXT4_FEATURE_REQUIRED_64BIT){
        ExcludeBitmap |= ((uint64_t)descriptor->exclude_bitmap_hi << 32);
    }
    return ExcludeBitmap;
}

uint64_t mount_info_t::GetBlockBitmapCsum(ext4_group_descriptor_t* descriptor){
    uint64_t BlockBitmapCsum = (uint64_t)descriptor->block_bitmap_csum_lo;
    if(RequiredFeature & EXT4_FEATURE_REQUIRED_64BIT){
        BlockBitmapCsum |= ((uint64_t)descriptor->block_bitmap_csum_hi << 32);
    }
    return BlockBitmapCsum;
}

uint64_t mount_info_t::GetInodeBitmapCsum(ext4_group_descriptor_t* descriptor){
    uint64_t InodeBitmapCsum = (uint64_t)descriptor->inode_bitmap_csum_lo;
    if(RequiredFeature & EXT4_FEATURE_REQUIRED_64BIT){
        InodeBitmapCsum |= ((uint64_t)descriptor->inode_bitmap_csum_hi << 32);
    }
    return InodeBitmapCsum;
}

uint64_t mount_info_t::GetItableUnused(ext4_group_descriptor_t* descriptor){
    uint64_t ItableUnused = (uint64_t)descriptor->itable_unused_lo;
    if(RequiredFeature & EXT4_FEATURE_REQUIRED_64BIT){
        ItableUnused |= ((uint64_t)descriptor->itable_unused_hi << 32);
    }
    return ItableUnused;
}


/* Inode read */
KResult mount_info_t::ReadInode(ext4_inode_t* inode, uintptr_t buffer, uint64_t start, size64_t size){
    uint64_t ReadLimit = start + size;
    uint64_t ReadLimitBlock = GetNextBlockLocation(ReadLimit);
    if(ReadLimitBlock > inode->blocks){
        return KFAIL;
    }
    
    uint64_t StartBlock = GetBlockFromLocation(start);
    uint64_t NumberOfBlockToRead = ReadLimitBlock - StartBlock;
    uint64_t BufferPosition = (uint64_t)buffer;
    if(start % BlockSize){
        ReadInodeBlock(inode, (uintptr_t)BufferPosition, StartBlock, start % BlockSize, BlockSize - (start % BlockSize));
        BufferPosition += start % BlockSize;
        NumberOfBlockToRead--;
        StartBlock++;
    }
    for(uint64_t i = 0; i < NumberOfBlockToRead; i++){
        ReadInodeBlock(inode, (uintptr_t)BufferPosition, StartBlock + i, NULL, BlockSize);
        BufferPosition += BlockSize;
    }
}

KResult mount_info_t::ReadInodeBlock(ext4_inode_t* inode, uintptr_t buffer, uint64_t block, uint64_t start, size64_t size){
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

                    uint64_t BlockRedirection2ToRead = BlockBuffer[(block % DoubleRedirectionLimit) / DoubleRedirectionLimit];
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



KResult mount_info_t::ReadBlock(uintptr_t buffer, uint64_t block, uint64_t start, size64_t size){
    uint64_t StartByte = GetLocationFromBlock(block) + start;
    return Srv_ReadDevice(StorageDevice, buffer, StartByte, size);
}



uint64_t mount_info_t::GetLocationFromInode(uint64_t inode){
    ext4_group_descriptor_t* DescriptorGroup = (ext4_group_descriptor_t*)malloc(sizeof(ext4_group_descriptor_t));
    uint64_t DescriptorGroupBlock = GetBlockGroupStartBlock(GetBlockGroupFromInode(inode)) + SuperBlock->first_data_block;
    uint64_t DescriptorGroupOffset = GetLocationFromBlock(DescriptorGroupBlock);

    Srv_ReadDevice(StorageDevice, DescriptorGroup, DescriptorGroupOffset, sizeof(ext4_group_descriptor_t));

    uint64_t InodeTableBlock = (uint64_t)DescriptorGroup->inode_table_lo + ((uint64_t)DescriptorGroup->inode_table_hi << 32);
    uint64_t LocationOfInode = DescriptorGroupOffset + GetIndexInodeInsideBlockGroupFromInode(inode) * InodeSize;
    return LocationOfInode;
}
