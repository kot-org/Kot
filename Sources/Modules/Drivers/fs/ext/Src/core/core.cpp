#include <core/core.h>

KResult InitializeMount(srv_storage_device_t* StorageDevice){
    mount_info_t* MountInfo = (mount_info_t*)malloc(sizeof(mount_info_t));
    MountInfo->SuperBlock = (super_block_t*)malloc(sizeof(super_block_t));
    Srv_ReadDevice(StorageDevice, MountInfo->SuperBlock, EXT2_SUPERBLOCK_START, sizeof(super_block_t));

    if(MountInfo->SuperBlock->magic != EXT2_SUPER_MAGIC){
        free(MountInfo);
        free(MountInfo->SuperBlock);
        return KFAIL;
    }

    MountInfo->SuperBlockDynamic = (super_block_ext4_dynamic_t*)malloc(sizeof(super_block_ext4_dynamic_t));
    Srv_ReadDevice(StorageDevice, MountInfo->SuperBlockDynamic, EXT2_SUPERBLOCK_START + sizeof(super_block_t), sizeof(super_block_ext4_dynamic_t));

    MountInfo->StorageDevice = StorageDevice;

    MountInfo->BlockSize = EXT2_LEFT_VALUE_TO_SHIFT_LEFT << MountInfo->SuperBlock->log_block_size;
    MountInfo->FirstBlock = (EXT2_SUPERBLOCK_SIZE / MountInfo->BlockSize) + 1;
    ext4_inode_t* Inode = (ext4_inode_t*)malloc(sizeof(ext4_inode_t));
    for(uint64_t i = 10; i < 20; i++){
        ext4_group_descriptor_t* DescriptorGroup = (ext4_group_descriptor_t*)malloc(sizeof(ext4_group_descriptor_t));
        uint64_t DescriptorGroupBlock = MountInfo->GetBlockGroupStartBlock(MountInfo->GetBlockGroupFromInode(i)) + MountInfo->FirstBlock;
        uint64_t DescriptorGroupOffset = MountInfo->GetLocationFromBlock(DescriptorGroupBlock);

        Srv_ReadDevice(StorageDevice, DescriptorGroup, DescriptorGroupOffset, sizeof(ext4_group_descriptor_t));

        uint64_t InodeTableBlock = (uint64_t)DescriptorGroup->inode_table_lo | ((uint64_t)DescriptorGroup->inode_table_hi << 32); ;
        uint64_t LocationOfInode = MountInfo->GetLocationFromBlock(InodeTableBlock) + MountInfo->GetIndexInodeInsideBlockGroupFromInode(i) * MountInfo->SuperBlockDynamic->inode_size;
        
        Srv_ReadDevice(StorageDevice, Inode, LocationOfInode, sizeof(ext4_inode_t));
        std::printf("%x", Inode->blocks);
    }
    return KSUCCESS;
}

uint64_t mount_info_t::GetLocationFromBlock(uint64_t block){
    return BlockSize * block;
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

uint64_t mount_info_t::GetLocationFromInode(uint64_t inode){
    ext4_group_descriptor_t* DescriptorGroup = (ext4_group_descriptor_t*)malloc(sizeof(ext4_group_descriptor_t));
    uint64_t DescriptorGroupBlock = GetBlockGroupStartBlock(GetBlockGroupFromInode(inode)) + SuperBlock->first_data_block;
    uint64_t DescriptorGroupOffset = GetLocationFromBlock(DescriptorGroupBlock);

    Srv_ReadDevice(StorageDevice, DescriptorGroup, DescriptorGroupOffset, sizeof(ext4_group_descriptor_t));

    uint64_t InodeTableBlock = (uint64_t)DescriptorGroup->inode_table_lo + ((uint64_t)DescriptorGroup->inode_table_hi << 32);
    uint64_t LocationOfInode = DescriptorGroupOffset + GetIndexInodeInsideBlockGroupFromInode(inode) * SuperBlockDynamic->inode_size;
    return LocationOfInode;
}