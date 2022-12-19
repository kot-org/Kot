#pragma once

#include <kot/bits.h>
#include <kot/math.h>
#include <main/main.h>
#include <kot/uisd/srvs/time.h>
#include <kot/uisd/srvs/storage.h>
#include <kot/uisd/srvs/storage/device.h>

#include <kot++/printf.h>
#include <kot++/string.h>

#define EXT_SUPER_MAGIC 				0xEF53
#define EXT_SUPERBLOCK_START 			0x400
#define EXT_SUPERBLOCK_SIZE 			0x400
#define EXT_LEFT_VALUE_TO_SHIFT_LEFT 	0x400
#define EXT_N_BLOCKS 					0xF

#define EXT_GOOD_OLD_REV				0x0
#define EXT_DYNAMIC_REV					0x1 
#define EXT_GOOD_OLD_INODE_SIZE 		0x80

#define EXT_GOOD_OLD_FIRST_INO			0xB

#define	EXT_BAD_INO		 				0x1	/* Bad blocks inode */
#define EXT_ROOT_INO		 			0x2	/* Root inode */
#define EXT_USR_QUOTA_INO	 			0x3	/* User quota inode */
#define EXT_GRP_QUOTA_INO	 			0x4	/* Group quota inode */
#define EXT_BOOT_LOADER_INO	 			0x5	/* Boot loader inode */
#define EXT_UNDEL_DIR_INO	 			0x6	/* Undelete directory inode */
#define EXT_RESIZE_INO		 			0x7	/* Reserved group descriptors inode */
#define EXT_JOURNAL_INO	 				0x8	/* Journal inode */

struct super_block_t{
    uint32_t inodes_count;
    uint32_t blocks_count;
    uint32_t r_blocks_count;
    uint32_t free_blocks_count;
    uint32_t free_inodes_count;
    uint32_t first_data_block;
    uint32_t log_block_size;
    uint32_t log_frag_size;
    uint32_t blocks_per_group;
    uint32_t frags_per_group;
    uint32_t inodes_per_group;
    uint32_t mtime;
    uint32_t wtime;
    uint16_t mnt_count;
    uint16_t max_mnt_count;
    uint16_t magic;
    uint16_t state;
    uint16_t errors;
    uint16_t minor_rev_level;
    uint32_t lastcheck;
    uint32_t checkinterval;
    uint32_t creator_os;
    uint32_t rev_level;
    uint16_t def_resuid;
    uint16_t def_resgid;
}__attribute__((packed));

struct super_block_ext2_dynamic_t{
    uint32_t first_ino; /* First non-reserved inode in file system */
    uint16_t inode_size; /* Size of each inode structure in bytes */
    uint16_t block_group_nr; /*	Block group that this superblock is part of for backup copies */
    uint32_t optional_features; /* Optional features present */
    uint32_t required_features; /* Required features present */
    uint32_t mro_features; /* Features that if not supported the volume must be mounted read-only */
    UUID_t uuid; /*	File system UUID */
    char volume_name[16]; /* Volume name */
    char last_mounted[64]; /* Path Volume was last mounted to */
    uint32_t algo_bitmap; /* Compression algorithm used*/
    uint8_t prealloc_blocks; /* Amount of blocks to preallocate for files */
    uint8_t prealloc_dir_blocks; /* Amount of blocks to preallocate for directories */
    uint8_t reserved_GDT_filesystem_expansion; /* Amount of reserved GDT entries for filesystem expansion */
    UUID_t journal_uuid; /* Journal UUID */
    uint32_t journal_inum; /* Journal Inode */
    uint32_t journal_dev; /* Journal Device number */
    uint32_t last_orphan; /* Head of orphan inode list */
}__attribute__((packed));

/* Required features */
#define EXT2_FEATURE_REQUIRED_COMPRESSION	0x0001
#define EXT2_FEATURE_REQUIRED_FILETYPE		0x0002
#define EXT2_FEATURE_REQUIRED_RECOVER		0x0004
#define EXT2_FEATURE_REQUIRED_JOURNAL_DEV	0x0008

/* Optional features */
#define EXT2_FEATURE_OPTIONAL_DIR_PREALLOC	0x0001
#define EXT2_FEATURE_OPTIONAL_IMAGIC_INODES	0x0002
#define EXT2_FEATURE_OPTIONAL_HAS_JOURNAL	0x0004
#define EXT2_FEATURE_OPTIONAL_EXT_ATTR		0x0008
#define EXT2_FEATURE_OPTIONAL_RESIZE_INODE	0x0010
#define EXT2_FEATURE_OPTIONAL_DIR_INDEX		0x0020

struct ext2_group_descriptor_t{  	/* block group which contain blocks*/
	uint32_t block_bitmap;	/* 32bits of block address of block usage bitmap */
	uint32_t inode_bitmap;	/* 32bits of block address of inode usage bitmap */
	uint32_t inode_table;	/* 32bits of starting block address of inode table */
	uint16_t free_blocks_count; /* 16bits of number of unallocated blocks in group. */
	uint16_t free_inodes_count; /* 16bits of number of unallocated inodes in group */
	uint16_t used_dirs_count;	/* 16bits of number of directories in group */
}__attribute__((packed));

struct ext2_inode_t{
    uint16_t mode;
    uint16_t uid;
    uint32_t size_lo;
    uint32_t atime;
    uint32_t ctime;
    uint32_t mtime;
    uint32_t dtime;
    uint16_t gid;
    uint16_t links_count;
    uint32_t blocks_lo;
    uint32_t flags;
    uint32_t osd1;
    uint32_t block[EXT_N_BLOCKS]; 
	/* block[EXT_N_BLOCKS] : 
		- 0 to 11 : direct pointer to data block
		- 12 : pointer to block table (with 4 bytes per entries) -> data
		- 13 : pointer to block table (with 4 bytes per entries) -> pointer to block table (with 4 bytes per entries) -> data
		- 14 : pointer to block table (with 4 bytes per entries) -> pointer to block table (with 4 bytes per entries) -> pointer to block table (with 4 bytes per entries) -> data
	*/
    uint32_t generation;
    uint32_t file_acl;
    uint32_t size_hi;
    uint32_t faddr;
    uint8_t osd2[12];
}__attribute__((packed));

/* Inode block limit */
#define INODE_BLOCK_POINTER_DIRECT_ACCESS   		0x0
#define INODE_BLOCK_POINTER_SINGLY_INDIRECT_ACCESS  0xC
#define INODE_BLOCK_POINTER_DOUBLY_INDIRECT_ACCESS  0xD
#define INODE_BLOCK_POINTER_TRIPLY_INDIRECT_ACCESS	0xE

#define INODE_BLOCK_MAX_INDIRECT_BLOCK 				0xB

/* Inode Type and Permissions */
#define INODE_TYPE_FIFO 				0x1000
#define INODE_TYPE_CHARACTER_DEVICE 	0x2000
#define INODE_TYPE_DIRECTORY 			0x4000
#define INODE_TYPE_BLOCK_DEVICE 		0x6000
#define INODE_TYPE_REGULAR_FILE 		0x8000
#define INODE_TYPE_SYMBOLIC_LINK 		0xA000
#define INODE_TYPE_UNIX_SOCKET 			0xC000


#define INODE_PERMISSION_OTHER_EXECUTE 	0x001
#define INODE_PERMISSION_OTHER_WRITE 	0x002
#define INODE_PERMISSION_OTHER_READ 	0x004

#define INODE_PERMISSION_GROUP_EXECUTE 	0x008
#define INODE_PERMISSION_GROUP_WRITE 	0x010
#define INODE_PERMISSION_GROUP_READ 	0x020

#define INODE_PERMISSION_USER_EXECUTE 	0x040
#define INODE_PERMISSION_USER_WRITE 	0x080
#define INODE_PERMISSION_USER_READ 		0x100

#define INODE_MODE_STICKY_BIT			0x200
#define INODE_GROUP_ID					0x400
#define INODE_USER_ID					0x800

/* Inode Flags */
#define INODE_FLAGS_SECURE_DELETION	 			0x00001
#define INODE_FLAGS_KEEP_COPY	 				0x00002
#define INODE_FLAGS_FILE_COMPRESSION			0x00004
#define INODE_FLAGS_SYNCHRONOUS_UPDATE			0x00008
#define INODE_FLAGS_IMMUTABLE_FILE				0x00010
#define INODE_FLAGS_APPEND_ONLY					0x00020
#define INODE_FLAGS_NOT_INCLUDE_DUMP			0x00040
#define INODE_FLAGS_DONT_UPDATE_ACCESS_TIME		0x00080
#define INODE_FLAGS_HASH_INDEX_DIRECTORY		0x10000
#define INODE_FLAGS_AFS_DIRECTORY				0x20000
#define INODE_FLAGS_JOURNAL_FILE_DATA			0x40000

struct ext2_directory_entry_t{
    uint32_t inode;
    uint16_t size;
    uint8_t name_length;
    uint8_t type_indicator;
    char name[];
}__attribute__((packed));


/* Directory Type and Permissions */
#define DIRECTORY_TYPE_REGULAR_FILE 		0x1
#define DIRECTORY_TYPE_DIRECTORY 			0x2
#define DIRECTORY_TYPE_CHARACTER_DEVICE 	0x3
#define DIRECTORY_TYPE_BLOCK_DEVICE 		0x4
#define DIRECTORY_TYPE_FIFO 				0x5
#define DIRECTORY_TYPE_UNIX_SOCKET 			0x6
#define DIRECTORY_TYPE_SYMBOLIC_LINK 		0x7


struct inode_t{
    uint64_t InodeLocation;
    struct ext2_inode_t Inode;
};

#define INODE_EXTRA_SIZE sizeof(inode_t) - sizeof(ext2_inode_t)

struct ext_directory_t{
    struct mount_info_t* MountInfo;
    struct inode_t* Inode;
    permissions_t Permissions;
    struct read_dir_data* ReadDir(uint64_t index);
    KResult CloseDir();
};

struct ext_file_t{
    struct mount_info_t* MountInfo;
    struct inode_t* Inode;
    size64_t Size;
    permissions_t Permissions;
    KResult ReadFile(uintptr_t buffer, uint64_t start, size64_t size);
    KResult WriteFile(uintptr_t buffer, uint64_t start, size64_t size, bool is_data_end);
    KResult CloseFile();
};

struct read_dir_data{
    char* Name;
    bool IsFile;
};

struct mount_info_t{
	struct srv_storage_device_t* StorageDevice;
    struct super_block_t* SuperBlock;

	uint32_t RequiredFeature;
	uint32_t OptionalFeature;

    uint64_t BlockSize;
    uint64_t FirstBlock;
	uint64_t InodeSize;
	uint64_t FirstInode;

    uint64_t Lock;

    /* Posix info */
    char* UserName;
    uint64_t UID;
    uint64_t GID;

    uint64_t GetSpace();
    uint64_t GetSpaceFree();

    KResult ReadSuperBlock();
    KResult WriteSuperBlock();

	uint64_t GetLocationFromBlock(uint64_t block);
	uint64_t GetBlockFromLocation(uint64_t location);
	uint64_t GetLocationInBlock(uint64_t location);
	uint64_t GetNextBlockLocation(uint64_t location);
	uint64_t GetBlockGroupStartBlock(uint64_t group);
    uint64_t GetBlockGroupFromBlock(uint64_t block);
    uint64_t GetBlockPositionInsideBlockGroupFromBlock(uint64_t block);
	uint64_t GetBlockGroupFromInode(uint64_t inode);
	uint64_t GetIndexInodeInsideBlockGroupFromInode(uint64_t inode);


	struct inode_t* GetInode(uint64_t position);
	KResult SetInode(struct inode_t* inode);

	size64_t GetSizeFromInode(struct inode_t* inode);
	KResult SetSizeFromInode(struct inode_t* inode, size64_t size);
    uint64_t GetDirectoryIndicatorFromInode(struct inode_t* inode);


	struct ext2_group_descriptor_t* GetDescriptorFromGroup(uint64_t group);
	struct ext2_group_descriptor_t* GetDescriptorFromBlock(uint64_t block);
	struct ext2_group_descriptor_t* GetDescriptorFromInode(uint64_t inode);
    KResult SetDescriptorFromGroup(uint64_t group, struct ext2_group_descriptor_t* descriptor);

	uint64_t GetBlockBitmap(struct ext2_group_descriptor_t* descriptor);
	uint64_t GetInodeBitmap(struct ext2_group_descriptor_t* descriptor);
	uint64_t GetInodeTable(struct ext2_group_descriptor_t* descriptor);
	uint64_t GetFreeBlocksCount(struct ext2_group_descriptor_t* descriptor);
	uint64_t GetFreeInodesCount(struct ext2_group_descriptor_t* descriptor);
	uint64_t GetUsedDirCount(struct ext2_group_descriptor_t* descriptor);

	KResult ReadInode(struct inode_t* inode, uintptr_t buffer, uint64_t start, size64_t size);
	KResult ReadInodeBlock(struct inode_t* inode, uintptr_t buffer, uint64_t block, uint64_t start, size64_t size);
	KResult WriteInode(struct inode_t* inode, uintptr_t buffer, uint64_t start, size64_t size, bool is_data_end);
	KResult WriteInodeBlock(struct inode_t* inode, uintptr_t buffer, uint64_t block, uint64_t start, size64_t size);

	KResult ReadBlock(uintptr_t buffer, uint64_t block, uint64_t start, size64_t size);
	KResult WriteBlock(uintptr_t buffer, uint64_t block, uint64_t start, size64_t size);

	uint64_t GetLocationFromInode(uint64_t inode);

    struct inode_t* FindInodeFromPath(char* path);
    struct inode_t* FindInodeFromInodeEntryAndPath(struct inode_t* inode, char* path);
    struct inode_t* FindInodeInodeAndEntryFromName(struct inode_t* inode, char* name);
    KResult LinkInodeToDirectory(struct inode_t* directory_inode, struct inode_t* inode, char* name);
    KResult UnlinkInodeToDirectory(struct inode_t* inode, char* name);

    KResult AllocateBlock(uint64_t* block);
    KResult FreeBlock(uint64_t block);

    KResult AllocateInode(uint64_t* inode);
    KResult FreeInode(uint64_t inode);
    KResult DeleteInode(struct inode_t* inode);

    KResult AllocateInodeBlocks(struct inode_t* inode, size64_t block_size, uint64_t block);
    KResult FreeInodeBlocks(struct inode_t* inode, size64_t block_size, uint64_t block);

    KResult AllocateInodeBlock(struct inode_t* inode, uint64_t block);
    KResult FreeInodeBlock(struct inode_t* inode, uint64_t block);

    KResult CheckPermissions(inode_t* inode, permissions_t permissions, permissions_t permissions_requested);

    KResult CreateDir(char* path, permissions_t permissions);
    KResult RemoveDir(char* path, permissions_t permissions);
    struct ext_directory_t* OpenDir(char* path, permissions_t permissions);
    struct ext_directory_t* OpenDir(struct inode_t* inode, char* path, permissions_t permissions);
    
    KResult Rename(char* old_path, char* new_path, permissions_t permissions);

    KResult CreateFile(char* path, char* name, permissions_t permissions);
    KResult RemoveFile(char* path, permissions_t permissions);
    struct ext_file_t* OpenFile(char* path, permissions_t permissions);
    struct ext_file_t* OpenFile(inode_t* inode, char* path, permissions_t permissions);
};


struct mount_info_t* InitializeMount(struct srv_storage_device_t* StorageDevice);