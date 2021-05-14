#pragma once

#include <cstdint>

constexpr uint16_t TFS_BLOCK_SIZE = 512;

constexpr uint16_t TFS_MAGIC = 3745;

constexpr uint8_t TFS_HEADER_BLOCK = 0;
constexpr uint8_t TFS_ALLOCATION_BLOCK = 1;
constexpr uint8_t TFS_DIRECTORY_BLOCK = 2;

constexpr uint8_t TFS_VOLNAME_MAX = 16;
constexpr uint8_t TFS_FILENAME_MAX = 16;

constexpr uint16_t TFS_BLOCKS_MAX = (TFS_BLOCK_SIZE / sizeof(uint32_t)) - 1;

constexpr uint16_t TFS_MAX_FILESIZE = (TFS_BLOCK_SIZE * TFS_BLOCKS_MAX);

typedef struct {
    uint32_t filesize;
    uint32_t block[TFS_BLOCKS_MAX];
} tfs_inode_t;
static_assert(sizeof(tfs_inode_t) == sizeof(uint32_t) * 128);

typedef struct {
    uint32_t inode;
    char name[TFS_FILENAME_MAX];
} tfs_direntry_t;

constexpr uint16_t TFS_MAX_FILES = TFS_BLOCK_SIZE / sizeof(tfs_direntry_t);