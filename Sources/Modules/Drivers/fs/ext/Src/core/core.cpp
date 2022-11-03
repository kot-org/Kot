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

    std::printf("%x", MountInfo->SuperBlock->log_block_size << 10);
    return KSUCCESS;
}