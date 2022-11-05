#pragma once

#include <kot/bits.h>
#include <kot/math.h>
#include <main/main.h>
#include <kot/uisd/srvs/storage.h>
#include <kot/uisd/srvs/storage/device.h>

#include <kot++/printf.h>

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

struct super_block_ext4_dynamic_t{
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
    uint32_t hash_seed[32]; /* HTREE hash seed in an array of 32 bit integers */
    uint8_t def_hash_version; /* Hash algorithm to use for directories */
    uint8_t journal_block_field; /* Journal blocks field contains a copy of the inode's block array and size */
    uint16_t size_group_descriptor; /* Size of group descriptors in bytes, for 64 bit mode */
    uint32_t default_mount_options; /* Mount options */
    uint32_t first_metablock_group; /* First metablock block group, if enabled */
    uint32_t filesystem_creation_time; /* Filesystem Creation Time */
    uint8_t journal_inode_backup[68]; /* Journal Inode Backup in an array of 32 bit integers*/

    /* Valid if the 64bit feature is set */
    uint32_t blocks_count_hi;	/* High 32-bits of the total number of blocks */
	uint32_t r_blocks_count_hi;	/* High 32-bits of the total number of reserved blocks */
	uint32_t free_blocks_count_hi;	/* High 32-bits of the total number of unallocated blocks */
	uint16_t min_extra_isize;	/* Minimum inode size */
	uint16_t want_extra_isize; 	/* Minimum inode reservation size */
	uint32_t flags;		/* Misc flags, such as sign of directory hash or development status */
	uint16_t raid_stride;		/* Amount logical blocks read or written per disk in a RAID array. */
	uint16_t mmp_update_interval;  /* Amount of seconds to wait in Multi-mount prevention checking */
	uint64_t mmp_block;            /* Block to multi-mount prevent */
	uint32_t raid_stripe_width;    /* Amount of blocks to read or write before returning to the current disk in a RAID array. Amount of disks * stride */
	uint8_t log_groups_per_flex;  /* log2 (groups per flex) - 10. (In other words, the number to shift 1,024 to the left by to obtain the groups per flex block group) */
	uint8_t checksum_type;	/* Metadata checksum algorithm used. Linux only supports crc32 */
	uint8_t encryption_level;	/* Encryption version level */
	uint8_t reserved_pad;		/* Padding to next 32bits */
	uint64_t kbytes_written;	/* Amount of kilobytes written over the filesystem's lifetime */
	uint32_t snapshot_inum;	/* Inode number of the active snapshot */
	uint32_t snapshot_id;		/* Sequential ID of active snapshot */
	uint64_t snapshot_r_blocks_count; /* Reserved blocks for active  snapshot's future use */
	uint32_t snapshot_list;	/* Inode number of the head of the on-disk snapshot list */
	uint32_t error_count;		/* Amount of errors detected */
	uint32_t first_error_time;	/* First time an error occurred in POSIX time */
	uint32_t first_error_ino;	/* Inode number in the first error */
	uint64_t first_error_block;	/* Block number in the first error */
	uint8_t first_error_func[32];	/* Function where the first error occurred */
	uint32_t first_error_line;	/* Line number where the first error occurred */
	uint32_t last_error_time;	/* Most recent time an error occurred in POSIX time */
	uint32_t last_error_ino;	/* Inode number in the last error */
	uint32_t last_error_line;	/* Line number where the most recent error occurred */
	uint64_t last_error_block;	/* block involved of last error */
	uint8_t last_error_func[32];    /* Function where the most recent error occurred */
	uint8_t mount_opts[64]; /* Mount options. (C-style string: characters terminated by a 0 byte) */
	uint32_t usr_quota_inum;	/* Inode number for user quota file */
	uint32_t grp_quota_inum;	/* Inode number for group quota file */
	uint32_t overhead_clusters;	/* Overhead blocks/clusters in filesystem. Zero means the kernel calculates it at runtime */
	uint32_t backup_bgs[2];	/* Block groups with backup Superblocks, if the sparse superblock flag is set. */
	uint8_t encrypt_algos[4];	/* Encryption algorithms used, as a array of unsigned char */
	uint8_t encrypt_pw_salt[16];	/* Salt used for string2key algorithm */
	uint32_t lpf_ino;		/* Inode number of the lost+found directory */
	uint32_t prj_quota_inum;	/* Inode number of the project quota tracker */
	uint32_t checksum_seed;	/* Checksum of the UUID, used for the checksum seed (crc32c(~0, UUID)) */
	uint8_t wtime_hi; /* High 8-bits of the last written time field */
	uint8_t mtime_hi; /* High 8-bits of the last mount time field */
	uint8_t mkfs_time_hi; /* High 8-bits of the Filesystem creation time field */
	uint8_t lastcheck_hi; /* High 8-bits of the last consistency check time field */
	uint8_t first_error_time_hi; /* High 8-bits of the first time an error occurred time field */
	uint8_t last_error_time_hi; /* High 8-bits of the latest time an error occurred time field */
	uint8_t first_error_errcode; /* Error code of the first error */
	uint8_t last_error_errcode; /* Error code of the latest error */
	uint16_t encoding;		/* Filename charset encoding */
	uint16_t encoding_flags;	/* Filename charset encoding flags */
	uint32_t orphan_file_inum;	/* Inode for tracking orphan inodes */
	uint32_t reserved[94];		/* Padding to the end of the block */
	uint32_t checksum;		/* Checksum of the superblock crc32c(superblock) */
}__attribute__((packed));

/* Required features */
#define EXT4_FEATURE_REQUIRED_COMPRESSION	0x0001
#define EXT4_FEATURE_REQUIRED_FILETYPE		0x0002
#define EXT4_FEATURE_REQUIRED_RECOVER		0x0004
#define EXT4_FEATURE_REQUIRED_JOURNAL_DEV	0x0008
#define EXT4_FEATURE_REQUIRED_META_BG		0x0010
#define EXT4_FEATURE_REQUIRED_EXTENTS		0x0040
#define EXT4_FEATURE_REQUIRED_64BIT		    0x0080
#define EXT4_FEATURE_REQUIRED_MMP           0x0100
#define EXT4_FEATURE_REQUIRED_FLEX_BG		0x0200
#define EXT4_FEATURE_REQUIRED_EA_INODE		0x0400
#define EXT4_FEATURE_REQUIRED_DIRDATA		0x1000
#define EXT4_FEATURE_REQUIRED_CSUM_SEED		0x2000
#define EXT4_FEATURE_REQUIRED_LARGEDIR		0x4000
#define EXT4_FEATURE_REQUIRED_INLINE_DATA	0x8000
#define EXT4_FEATURE_REQUIRED_ENCRYPT		0x10000
#define EXT4_FEATURE_REQUIRED_CASEFOLD		0x20000

/* Optional features */
#define EXT4_FEATURE_OPTIONAL_DIR_PREALLOC	0x0001
#define EXT4_FEATURE_OPTIONAL_IMAGIC_INODES	0x0002
#define EXT4_FEATURE_OPTIONAL_HAS_JOURNAL		0x0004
#define EXT4_FEATURE_OPTIONAL_EXT_ATTR		0x0008
#define EXT4_FEATURE_OPTIONAL_RESIZE_INODE	0x0010
#define EXT4_FEATURE_OPTIONAL_DIR_INDEX		0x0020
#define EXT4_FEATURE_OPTIONAL_SPARSE_SUPER2	0x0200
#define EXT4_FEATURE_OPTIONAL_FAST_COMMIT		0x0400
#define EXT4_FEATURE_OPTIONAL_STABLE_INODES	0x0800

struct ext4_flex_groups_t{
	uint64_t free_clusters;
	uint32_t free_inodes;
	uint32_t used_dirs;
}__attribute__((packed));

struct ext4_group_descriptor_t{  	/* block group which contain blocks*/
	uint32_t block_bitmap_lo;	/* Low 32bits of block address of block usage bitmap */
	uint32_t inode_bitmap_lo;	/* Low 32bits of block address of inode usage bitmap */
	uint32_t inode_table_lo;	/* Low 32bits of starting block address of inode table */
	uint16_t free_blocks_count_lo; /* Low 16bits of number of unallocated blocks in group. */
	uint16_t free_inodes_count_lo; /* Low 16bits of number of unallocated inodes in group */
	uint16_t used_dirs_count_lo;	/* Low 16bits of number of directories in group */
	uint16_t flags;		/* Block group features present */
	uint32_t exclude_bitmap_lo;   /* Low 32-bits of block address of snapshot exclude bitmap */
	uint16_t block_bitmap_csum_lo;/* Low 16-bits of Checksum of the block usage bitmap */
	uint16_t inode_bitmap_csum_lo;/* Low 16-bits of Checksum of the inode usage bitmap */
	uint16_t itable_unused_lo;	/* Low 16-bits of amount of free inodes */
	uint16_t checksum;		/* crc16(sb_uuid+group+desc) */
	uint32_t block_bitmap_hi;	/* High 32-bits of block address of block usage bitmap */
	uint32_t inode_bitmap_hi;	/* High 32-bits of block address of inode usage bitmap */
	uint32_t inode_table_hi;	/* High 32-bits of starting block address of inode table */
	uint16_t free_blocks_count_hi; /* High 16-bits of number of unallocated blocks in group */
	uint16_t free_inodes_count_hi; /* High 16-bits of number of unallocated inodes in group */
	uint16_t used_dirs_count_hi;	/* High 16-bits of number of directories in group */
	uint16_t itable_unused_hi;    /* High 16-bits of amount of free inodes */
	uint32_t exclude_bitmap_hi;   /* High 32-bits of block address of snapshot exclude bitmap */
	uint16_t block_bitmap_csum_hi; /* crc32c(s_uuid+grp_num+bbitmap) BE */
	uint16_t inode_bitmap_csum_hi; /* crc32c(s_uuid+grp_num+ibitmap) BE */
	uint32_t reserved;
}__attribute__((packed));

/* Block group flags */
#define EXT4_BG_INODE_UNINIT	0x0001 /* Inode table/bitmap not in use */
#define EXT4_BG_BLOCK_UNINIT	0x0002 /* Block bitmap not in use */
#define EXT4_BG_INODE_ZEROED	0x0004 /* On-disk itable initialized to zero */

struct ext4_inode_t{
    uint16_t mode;
    uint16_t uid;
    uint32_t size_lo;
    uint32_t atime;
    uint32_t ctime;
    uint32_t mtime;
    uint32_t dtime;
    uint16_t gid;
    uint16_t links_count;
    uint32_t blocks;
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
    uint32_t osd2[3];
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

struct ext4_directory_entry_t{
    uint32_t inode;
    uint16_t size;
    uint8_t name_length;
    uint8_t type_indicator;
    char name[];
}__attribute__((packed));

struct ext4_mmp_t{
	uint32_t mmp_magic;		/* Magic number for MMP */
	uint32_t mmp_seq;		/* Sequence no. updated periodically */
	uint64_t mmp_time;		/* Time last updated */
	char mmp_nodename[64];	/* Node which last updated MMP block */
	char mmp_bdevname[32];	/* Bdev which last updated MMP block */
	uint16_t mmp_check_interval;

	uint16_t mmp_pad1;
	uint32_t mmp_pad2[226];
	uint32_t mmp_checksum;		/* crc32c(uuid+mmp_block) */
}__attribute__((packed));

#define EXT4_MMP_MAGIC     0x004D4D50 /* ASCII for MMP */
#define EXT4_MMP_SEQ_CLEAN 0xFF4D4D50 /* mmp_seq value for clean unmount */
#define EXT4_MMP_SEQ_FSCK  0xE24D4D50 /* mmp_seq value when being fscked */
#define EXT4_MMP_SEQ_MAX   0xE24D4D4F /* maximum valid mmp_seq value */ 

struct mount_info_t{
	struct srv_storage_device_t* StorageDevice;
    struct super_block_t* SuperBlock;

	uint32_t RequiredFeature;
	uint32_t OptionalFeature;

    uint64_t BlockSize;
    uint64_t FirstBlock;
	uint64_t InodeSize;
	uint64_t FirstInode;

	uint64_t GetLocationFromBlock(uint64_t block);
	uint64_t GetBlockFromLocation(uint64_t location);
	uint64_t GetLocationInBlock(uint64_t location);
	uint64_t GetNextBlockLocation(uint64_t location);
	uint64_t GetBlockGroupStartBlock(uint64_t group);
	uint64_t GetBlockGroupFromInode(uint64_t inode);
	uint64_t GetIndexInodeInsideBlockGroupFromInode(uint64_t inode);


	struct ext4_inode_t* GetInode(uint64_t inode);

	uint64_t GetSizeFromInode(struct ext4_inode_t* inode);


	struct ext4_group_descriptor_t* GetDescriptorFromInode(uint64_t inode);

	uint64_t GetBlockBitmap(struct ext4_group_descriptor_t* descriptor);
	uint64_t GetInodeBitmap(struct ext4_group_descriptor_t* descriptor);
	uint64_t GetInodeTable(struct ext4_group_descriptor_t* descriptor);
	uint64_t GetFreeBlocksCount(struct ext4_group_descriptor_t* descriptor);
	uint64_t GetFreeInodesCount(struct ext4_group_descriptor_t* descriptor);
	uint64_t GetUsedDirCount(struct ext4_group_descriptor_t* descriptor);
	uint64_t GetExcludeBitmap(struct ext4_group_descriptor_t* descriptor);
	uint64_t GetBlockBitmapCsum(struct ext4_group_descriptor_t* descriptor);
	uint64_t GetInodeBitmapCsum(struct ext4_group_descriptor_t* descriptor);
	uint64_t GetItableUnused(struct ext4_group_descriptor_t* descriptor);

	KResult ReadInode(ext4_inode_t* inode, uintptr_t buffer, uint64_t start, size64_t size);
	KResult ReadInodeBlock(ext4_inode_t* inode, uintptr_t buffer, uint64_t block, uint64_t start, size64_t size);
	KResult ReadBlock(uintptr_t buffer, uint64_t block, uint64_t start, size64_t size);

	uint64_t GetLocationFromInode(uint64_t inode);
};


KResult InitializeMount(struct srv_storage_device_t* StorageDevice);