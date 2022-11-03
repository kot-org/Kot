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

struct super_block_ext2_dynamic{
    uint32_t first_ino;
    uint16_t inode_size;
    uint16_t block_group_nr;
    uint32_t feature_compat;
    uint32_t feature_incompat;
    uint32_t feature_ro_compat;
    UUID_t uuid;
    char volume_name[16];
    char last_mounted[64];
    uint32_t algo_bitmap;
    uint8_t prealloc_blocks;
    uint8_t prealloc_dir_blocks;
    uint16_t reserved0; // reserved for alignement
    UUID_t journal_uuid;
    uint32_t journal_inum;
    uint32_t journal_dev;
    uint32_t last_orphan;
    uint32_t hash_seed[32];
    uint8_t def_hash_version;
    uint32_t reserved1:24; // reserved for future expansion
    uint32_t default_mount_options;
    uint32_t first_meta_bg;
    uint8_t reserved2[760]; // reserved for future revisions
}__attribute__((packed));

struct super_block_ext4_dynamic{
    uint32_t first_ino;
    uint16_t inode_size;
    uint16_t block_group_nr;
    uint32_t optional_features;
    uint32_t required_features;
    uint32_t mro_features; // features available when the volume is mounted in read-only
    UUID_t uuid;
    char volume_name[16];
    char last_mounted[64];
    uint32_t algo_bitmap;
    uint8_t prealloc_blocks;
    uint8_t prealloc_dir_blocks;
    uint8_t reserved_GDT_filesystem_expansion;
    UUID_t journal_uuid;
    uint32_t journal_inum;
    uint32_t journal_dev;
    uint32_t last_orphan;
    uint32_t hash_seed[32];
    uint8_t def_hash_version;
    uint8_t journal_block_field;
    uint16_t size_group_descriptor; // for 64 bit mode
    uint32_t default_mount_options;
    uint32_t first_metablock_group;
    uint32_t filesystem_creation_time;
    uint8_t journal_inode_backup[68];
    /* 64bit support valid if EXT4_FEATURE_COMPAT_64BIT */
    uint32_t blocks_count_hi;	/* Blocks count */
	uint32_t r_blocks_count_hi;	/* Reserved blocks count */
	uint32_t free_blocks_count_hi;	/* Free blocks count */
	uint16_t min_extra_isize;	/* All inodes have at least # bytes */
	uint16_t want_extra_isize; 	/* New inodes should reserve # bytes */
	uint32_t flags;		/* Miscellaneous flags */
	uint16_t raid_stride;		/* RAID stride */
	uint16_t mmp_update_interval;  /* # seconds to wait in MMP checking */
	uint64_t mmp_block;            /* Block for multi-mount protection */
	uint32_t raid_stripe_width;    /* blocks on all data disks (N*stride)*/
	uint8_t log_groups_per_flex;  /* FLEX_BG group size */
	uint8_t checksum_type;	/* metadata checksum algorithm used */
	uint8_t encryption_level;	/* versioning level for encryption */
	uint8_t reserved_pad;		/* Padding to next 32bits */
	uint64_t kbytes_written;	/* nr of lifetime kilobytes written */
	uint32_t snapshot_inum;	/* Inode number of active snapshot */
	uint32_t snapshot_id;		/* sequential ID of active snapshot */
	uint64_t snapshot_r_blocks_count; /* reserved blocks for active  snapshot's future use */
	uint32_t snapshot_list;	/* inode number of the head of the on-disk snapshot list */
	uint32_t error_count;		/* number of fs errors */
	uint32_t first_error_time;	/* first time an error happened */
	uint32_t first_error_ino;	/* inode involved in first error */
	uint64_t first_error_block;	/* block involved of first error */
	uint8_t first_error_func[32];	/* function where the error happened */
	uint32_t first_error_line;	/* line number where error happened */
	uint32_t last_error_time;	/* most recent time of an error */
	uint32_t last_error_ino;	/* inode involved in last error */
	uint32_t last_error_line;	/* line number where error happened */
	uint64_t last_error_block;	/* block involved of last error */
	uint8_t last_error_func[32];	/* function where the error happened */
	uint8_t mount_opts[64];
	uint32_t usr_quota_inum;	/* inode for tracking user quota */
	uint32_t grp_quota_inum;	/* inode for tracking group quota */
	uint32_t overhead_clusters;	/* overhead blocks/clusters in fs */
	uint32_t backup_bgs[2];	/* groups with sparse_super2 SBs */
	uint8_t encrypt_algos[4];	/* Encryption algorithms in use  */
	uint8_t encrypt_pw_salt[16];	/* Salt used for string2key algorithm */
	uint32_t lpf_ino;		/* Location of the lost+found inode */
	uint32_t prj_quota_inum;	/* inode for tracking project quota */
	uint32_t checksum_seed;	/* crc32c(uuid) if csum_seed set */
	uint8_t wtime_hi;
	uint8_t mtime_hi;
	uint8_t mkfs_time_hi;
	uint8_t lastcheck_hi;
	uint8_t first_error_time_hi;
	uint8_t last_error_time_hi;
	uint8_t first_error_errcode;
	uint8_t last_error_errcode;
	uint16_t encoding;		/* Filename charset encoding */
	uint16_t encoding_flags;	/* Filename charset encoding flags */
	uint32_t orphan_file_inum;	/* Inode for tracking orphan inodes */
	uint32_t reserved[94];		/* Padding to the end of the block */
	uint32_t checksum;		/* crc32c(superblock) */
}__attribute__((packed));

struct mount_info_t{
    struct super_block_t* SuperBlock;
    struct srv_storage_device_t* StorageDevice;
};


KResult InitializeMount(struct srv_storage_device_t* StorageDevice);