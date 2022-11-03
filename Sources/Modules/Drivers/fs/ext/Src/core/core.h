#pragma once

#include <main/main.h>
#include <kot/uisd/srvs/storage.h>
#include <kot/uisd/srvs/storage/device.h>

#include <kot++/printf.h>

#define EXT2_SUPER_MAGIC 0xEF53
#define EXT2_SUPERBLOCK_START 0x400

struct super_block_t{
    uint32_t nodes_count;
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

struct super_block_ext4_dynamic{
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
#define EXT4_FEATURE_INCOMPAT_COMPRESSION	0x0001
#define EXT4_FEATURE_INCOMPAT_FILETYPE		0x0002
#define EXT4_FEATURE_INCOMPAT_RECOVER		0x0004
#define EXT4_FEATURE_INCOMPAT_JOURNAL_DEV	0x0008
#define EXT4_FEATURE_INCOMPAT_META_BG		0x0010
#define EXT4_FEATURE_INCOMPAT_EXTENTS		0x0040
#define EXT4_FEATURE_INCOMPAT_64BIT		    0x0080
#define EXT4_FEATURE_INCOMPAT_MMP           0x0100
#define EXT4_FEATURE_INCOMPAT_FLEX_BG		0x0200
#define EXT4_FEATURE_INCOMPAT_EA_INODE		0x0400
#define EXT4_FEATURE_INCOMPAT_DIRDATA		0x1000
#define EXT4_FEATURE_INCOMPAT_CSUM_SEED		0x2000
#define EXT4_FEATURE_INCOMPAT_LARGEDIR		0x4000
#define EXT4_FEATURE_INCOMPAT_INLINE_DATA	0x8000
#define EXT4_FEATURE_INCOMPAT_ENCRYPT		0x10000
#define EXT4_FEATURE_INCOMPAT_CASEFOLD		0x20000

/* Optional features */
#define EXT4_FEATURE_COMPAT_DIR_PREALLOC	0x0001
#define EXT4_FEATURE_COMPAT_IMAGIC_INODES	0x0002
#define EXT4_FEATURE_COMPAT_HAS_JOURNAL		0x0004
#define EXT4_FEATURE_COMPAT_EXT_ATTR		0x0008
#define EXT4_FEATURE_COMPAT_RESIZE_INODE	0x0010
#define EXT4_FEATURE_COMPAT_DIR_INDEX		0x0020
#define EXT4_FEATURE_COMPAT_SPARSE_SUPER2	0x0200
#define EXT4_FEATURE_COMPAT_FAST_COMMIT		0x0400
#define EXT4_FEATURE_COMPAT_STABLE_INODES	0x0800

struct ext4_flex_groups{
	uint64_t free_clusters;
	uint32_t free_inodes;
	uint32_t used_dirs;
}__attribute__((packed));

struct ext4_group_descriptor{
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


struct ext4_mmp_struct {
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

struct ext4_inode{
    uint16_t mode;
    uint16_t uid;
    uint32_t size;
    uint32_t atime;
    uint32_t ctime;
    uint32_t mtime;
    uint32_t dtime;
    uint16_t gid;
    uint16_t links_count;
    uint32_t blocks;
    uint32_t flags;
    uint32_t osd1;
    uint32_t block[15];
    uint32_t generation;
    uint32_t file_acl;
    uint32_t dir_acl;
    uint32_t faddr;
    uint32_t osd2[3];
}__attribute__((packed));

struct mount_info_t{
    struct super_block_t* SuperBlock;
    struct srv_storage_device_t* StorageDevice;
};


KResult InitializeMount(struct srv_storage_device_t* StorageDevice);