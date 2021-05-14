// Trivial Filesystem from KUDOS

#include "tfs.h"
#include "tfs_constants.h"
#include "drivers/gbd.h"
#include "Bitmap.h"
#include "paging/PageFrameAllocator.h"
#include "graphics/BasicRenderer.h"
#include "Panic.h"
#include "string.h"
#include "io/serial.h"

/*
The master data struct that is used throughout the implementation.  It contains everything needed
to perform TFS operations
*/
typedef struct {
    // Where on the disk does the filesystem start
    uint64_t startblock;

    // The number of blocks [sectors] the filesystem occupies
    uint64_t totalblocks;

    // A handle to the abstracted disk for reads, writes, etc
    gbd_t* disk;

    // A storage location for the file INODE data
    tfs_inode_t* buffer_inode;

    // A storage location for the block usage bitmap
    uint32_t*       buffer_bat;

    // A storage location for the directory entries
    tfs_direntry_t* buffer_md;
} tfs_t;

// Byte swaps a given number of 32-bit numbers in a buffer
static inline void swap32_multi(uint32_t* buf, size_t count) {
    while(count--) {
        *buf = __builtin_bswap32(*buf);
        buf++;
    }
}

static int tfs_unmount(fs_t* fs) {
    // The only thing to do currently is free the page that was allocated
    // to hold the tfs_t struct
    PageFrameAllocator::SharedAllocator()->FreePage((void *)fs);

    return VFS_OK;
}

static gbd_request_t create_read_block_req(uint64_t start, void* buffer) {
    gbd_request_t req;
    req.start = start;
    req.count = 1;
    req.operation = GBD_OPERATION_READ;
    req.buf = buffer;

    return req;
}

static gbd_request_t create_write_block_req(uint64_t start, void* buffer) {
    gbd_request_t req;
    req.start = start;
    req.count = 1;
    req.operation = GBD_OPERATION_WRITE;
    req.buf = buffer;

    return req;
}

static int tfs_open(fs_t* fs, const char* filename) {
    if(filename[0] == '/') {
        // Since TFS only has the root directory, skip any leading
        // forward slash
        filename++;
    }

    // Read the directory information so we can look for the file by name
    tfs_t* tfs = (tfs_t *)fs->internal;
    gbd_request_t req = create_read_block_req(tfs->startblock + TFS_DIRECTORY_BLOCK, tfs->buffer_md);
    int r = tfs->disk->read_block(tfs->disk, &req);
    if(r == 0) {
        uart_printf("!! tfs_open: read error at block 0x%x\r\n", TFS_DIRECTORY_BLOCK);
        return VFS_ERROR;
    }

    // Loop through the entries, looking for a match
    for(int i = 0; i < TFS_MAX_FILES; i++) {
        if(strncmp(tfs->buffer_md[i].name, filename, TFS_FILENAME_MAX) == 0) {
            return __builtin_bswap32(tfs->buffer_md[i].inode);
        }
    }

    uart_printf("tfs_open: file not found\r\n");
    return VFS_NOT_FOUND;
}

static int tfs_close(fs_t* fs, int fileid) {
    // No-op for TFS
    fs = fs;
    fileid = fileid;

    return VFS_OK;
}

static int tfs_create(fs_t* fs, const char* filename, int size, int mask) {
    if(filename[0] == '/') {
        // Since TFS only has the root directory, skip any leading
        // forward slash
        filename++;
    }

    tfs_t *tfs = (tfs_t *)fs->internal;

    // How many blocks do we need to store the file, given its size
    uint32_t numblocks = (size + TFS_BLOCK_SIZE - 1) / TFS_BLOCK_SIZE;
    if(numblocks > (TFS_BLOCK_SIZE / 4 - 1)) {
        return VFS_ERROR;
    }

    gbd_request_t req = create_read_block_req(tfs->startblock + TFS_DIRECTORY_BLOCK, tfs->buffer_md);
    int r = tfs->disk->read_block(tfs->disk, &req);
    if(r == 0) {
        return VFS_ERROR;
    }

    int index = -1;
    for(int i = 0; i < TFS_MAX_FILES; i++) {
        if(strncmp(tfs->buffer_md[i].name, filename, TFS_FILENAME_MAX) == 0) {
            // The file already exists
            return VFS_ERROR;
        }

        if(index == -1 && tfs->buffer_md[i].inode == 0 /* Big endian 0 and little endian 0 are the same */) {
            // Found a free inode to store the file information
            index = i;
        }
    }

    if(index == -1) {
        // No inode space available!
        return VFS_ERROR;
    }

    strncpy(tfs->buffer_md[index].name, filename, TFS_FILENAME_MAX);

    // Now to look at the allocation data, to see which blocks are free
    req = create_read_block_req(tfs->startblock + TFS_ALLOCATION_BLOCK, tfs->buffer_bat);
    r = tfs->disk->read_block(tfs->disk, &req);
    if(r == 0) {
        return VFS_ERROR;
    }

    tfs->buffer_md[index].inode = -1;
    swap32_multi(tfs->buffer_bat, tfs->totalblocks / 8 / 4);
    Bitmap bat = Bitmap(tfs->totalblocks / 8, tfs->buffer_bat);
    uint64_t next_index;
    if(!bat.SetNext(&next_index)) {
        // No blocks are free!!
        return VFS_ERROR;
    }

    // Store the fact that the file inode now exists at block 'next_index'
    tfs->buffer_md[index].inode = __builtin_bswap32((uint32_t)next_index);

    // Record the file size into the inode data
    tfs->buffer_inode->filesize = __builtin_bswap32(size);
    uint32_t i;
    for(i = 0; i < numblocks; i++) {
        // We also need to get the required number of blocks for the data
        if(!bat.SetNext(&next_index)) {
            // Oh no, we ran out!
            return VFS_ERROR;
        }

        // Record each block used for the data in the file inode
        tfs->buffer_inode->block[i] = __builtin_bswap32((uint32_t)next_index);
    }

    while(i < (TFS_BLOCK_SIZE / 4 - 1)) {
        // Clear out the ones that are not used by this file
        tfs->buffer_inode->block[i++] = 0;
    }

    // Now we have all we need to save back to the disk, so start with the block usage bitmap
    swap32_multi(tfs->buffer_bat, tfs->totalblocks / 8 / 4);
    req = create_write_block_req(tfs->startblock + TFS_ALLOCATION_BLOCK, tfs->buffer_bat);
    r = tfs->disk->write_block(tfs->disk, &req);
    if(r == 0) {
        return VFS_ERROR;
    }

    // Save the updated directory information
    req = create_write_block_req(tfs->startblock + TFS_DIRECTORY_BLOCK, tfs->buffer_md);
    r = tfs->disk->write_block(tfs->disk, &req);
    if(r == 0) {
        return VFS_ERROR;
    }

    // Save the file inode
    req = create_write_block_req(tfs->startblock + __builtin_bswap32(tfs->buffer_md[index].inode), tfs->buffer_inode);
    r = tfs->disk->write_block(tfs->disk, &req);
    if(r == 0) {
        return VFS_ERROR;
    }

    // Zero out all the data blocks for this file
    memset(tfs->buffer_bat, 0, TFS_BLOCK_SIZE);
    for(i = 0; i < numblocks; i++) {
        req = create_write_block_req(tfs->startblock + __builtin_bswap32(tfs->buffer_inode->block[i]), tfs->buffer_bat);
        if(r == 0) {
            return VFS_ERROR;
        }
    }

    return VFS_OK;
}

static int tfs_remove(fs_t* fs, const char* filename) {
    if(filename[0] == '/') {
        // Since TFS only has the root directory, skip any leading
        // forward slash
        filename++;
    }

    // Looking familiar?  Getting the directory to search for the file by name.
    tfs_t* tfs = (tfs_t *)fs->internal;
    gbd_request_t req = create_read_block_req(tfs->startblock + TFS_DIRECTORY_BLOCK, tfs->buffer_md);
    int r = tfs->disk->read_block(tfs->disk, &req);
    if(r == 0) {
        return VFS_ERROR;
    }

    int index = -1;
    for(int i = 0; i < TFS_MAX_FILES; i++) {
        if(strncmp(tfs->buffer_md[i].name, filename, TFS_FILENAME_MAX) == 0) {
            index = i;
            break;
        }
    }

    if(index == -1) {
        return VFS_NOT_FOUND;
    }

    // Read the used block information, so we can mark all the blocks as freed
    req = create_read_block_req(tfs->startblock + TFS_ALLOCATION_BLOCK, tfs->buffer_bat);
    r = tfs->disk->read_block(tfs->disk, &req);
    if(r == 0) {
        return VFS_ERROR;
    }

    // Read the file inode data so we know which data blocks to free
    req = create_read_block_req(tfs->startblock + __builtin_bswap32(tfs->buffer_md[index].inode), tfs->buffer_inode);
    r = tfs->disk->read_block(tfs->disk, &req);
    if(r == 0) {
        return VFS_ERROR;
    }

    // Mark the inode block, and all data blocks as unused
    swap32_multi(tfs->buffer_bat, tfs->totalblocks / 8 / 4);
    Bitmap bat(tfs->totalblocks / 8, tfs->buffer_bat);
    bat.Set(__builtin_bswap32(tfs->buffer_md[index].inode), false);
    int i = 0;
    while(tfs->buffer_inode->block[i] != 0 && i < (TFS_BLOCK_SIZE / 4 - 1)) {
        bat.Set(__builtin_bswap32(tfs->buffer_inode->block[i]), false);
        i++;
    }

    // Update the directory to indicate this file no longer exists
    tfs->buffer_md[index].inode = 0;
    tfs->buffer_md[index].name[0] = 0;

    // Write the updated used block bitmap
    swap32_multi(tfs->buffer_bat, tfs->totalblocks / 8 / 4);
    req = create_write_block_req(tfs->startblock + TFS_ALLOCATION_BLOCK, tfs->buffer_bat);
    r = tfs->disk->write_block(tfs->disk, &req);
    if(r == 0) {
        return VFS_ERROR;
    }

    // Write the updated directory
    req = create_write_block_req(tfs->startblock + TFS_DIRECTORY_BLOCK, tfs->buffer_md);
    r = tfs->disk->write_block(tfs->disk, &req);
    if(r == 0) {
        return VFS_ERROR;
    }

    return VFS_OK;
}

int tfs_read(fs_t* fs, int fileid, void* buffer, int bufsize, int offset) {
    tfs_t *tfs = (tfs_t *)fs->internal;
    if(fileid < 2 || fileid > (int)tfs->totalblocks) {
        return VFS_ERROR;
    }

    // Since we already have the file ID, we can skip the directory
    gbd_request_t req = create_read_block_req(tfs->startblock + fileid, tfs->buffer_inode);
    int r = tfs->disk->read_block(tfs->disk, &req);
    if(r == 0) {
        return VFS_ERROR;
    }

    if(offset < 0 || offset > __builtin_bswap32(tfs->buffer_inode->filesize)) {
        return VFS_ERROR;
    }

    // Only read what is available!
    int remaining = __builtin_bswap32(tfs->buffer_inode->filesize) - offset;
    if(bufsize > remaining) {
        bufsize = remaining;
    }

    if(bufsize == 0) {
        return 0;
    }

    // Figure out the index of the first and last data blocks to read
    int b1 = offset / TFS_BLOCK_SIZE;
    int b2 = (offset + bufsize - 1) / TFS_BLOCK_SIZE;

    // Read the first block, since it may not be a full block depending on the offset
    req = create_read_block_req(tfs->startblock + __builtin_bswap32(tfs->buffer_inode->block[b1]), tfs->buffer_bat);
    r = tfs->disk->read_block(tfs->disk, &req);
    if(r == 0) {
        return VFS_ERROR;
    }

    int read = bufsize;
    if(read > TFS_BLOCK_SIZE - (offset % TFS_BLOCK_SIZE)) {
        read = TFS_BLOCK_SIZE - (offset % TFS_BLOCK_SIZE);
    }

    memcpy(buffer, (const void *)((uintptr_t)tfs->buffer_bat + (offset % TFS_BLOCK_SIZE)), read);
    buffer = (void *)((uintptr_t)buffer + read);
    b1++;

    // Go through the rest of the blocks and do one of two things
    while(b1 <= b2) {
        req.start = tfs->startblock + __builtin_bswap32(tfs->buffer_inode->block[b1]);
        r = tfs->disk->read_block(tfs->disk, &req);
        if(r == 0) {
            return VFS_ERROR;
        }

        if(b1 == b2) {
            // This is the last block, and it might not be a full block depending on the requested size
            memcpy(buffer, (const void *)(uintptr_t)tfs->buffer_bat, bufsize - read);
            read += (bufsize - read);
        } else {
            // Middle blocks are always full blocks, so copy them without fuss
            memcpy(buffer, (const void *)(uintptr_t)tfs->buffer_bat, TFS_BLOCK_SIZE);
            read += TFS_BLOCK_SIZE;
            buffer = (void *)((uintptr_t)buffer + TFS_BLOCK_SIZE);
        }

        b1++;
    }

    return read;
}

static int tfs_write(fs_t* fs, int fileid, void* buffer, int datasize, int offset) {
    tfs_t *tfs = (tfs_t *)fs->internal;
    if(fileid < 2 || fileid > (int)tfs->totalblocks) {
        return VFS_ERROR;
    }

    // This logic is very very similar to tfs_read, so check tfs_read for some insight
    gbd_request_t req = create_read_block_req(tfs->startblock + fileid, tfs->buffer_inode);
    int r = tfs->disk->read_block(tfs->disk, &req);
    if(r == 0) {
        return VFS_ERROR;
    }

    if(offset < 0 || offset > __builtin_bswap32(tfs->buffer_inode->filesize)) {
        return VFS_ERROR;
    }

    uint32_t filesize = __builtin_bswap32(tfs->buffer_inode->filesize);
    int remaining = filesize- offset;
    if(datasize > remaining) {
        datasize = remaining;
    }

    if(datasize == 0) {
        return 0;
    }

    int b1 = offset / TFS_BLOCK_SIZE;
    int b2 = (offset+datasize+1) / TFS_BLOCK_SIZE;
    int written = TFS_BLOCK_SIZE - (offset % TFS_BLOCK_SIZE);
    if(written > datasize) {
        written = datasize;
    }

    if(written < TFS_BLOCK_SIZE) {
        // We are dealing with a partial block, so we need to read in the existing content
        // first and then overwrite part of it with the new contents
        req = create_read_block_req(tfs->startblock + __builtin_bswap32(tfs->buffer_inode->block[b1]), tfs->buffer_bat);
        r = tfs->disk->read_block(tfs->disk, &req);
        if(r == 0) {
            return VFS_ERROR;
        }
    }

    // Overwrite and save back
    memcpy((void *)(((uintptr_t)tfs->buffer_bat) + (offset % TFS_BLOCK_SIZE)), buffer, written);
    req = create_write_block_req(tfs->startblock + __builtin_bswap32(tfs->buffer_inode->block[b1]), tfs->buffer_bat);
    r = tfs->disk->write_block(tfs->disk, &req);
    if(r == 0) {
        return VFS_ERROR;
    }

    buffer = (void *)((uintptr_t)buffer + written);
    b1++;
    while(b1 <= b2) {
        if(b1 == b2) {
            if((datasize - written) < TFS_BLOCK_SIZE) {
                req.start = tfs->startblock + __builtin_bswap32(tfs->buffer_inode->block[b1]);
                r = tfs->disk->read_block(tfs->disk, &req);
                if(r == 0) {
                    return VFS_ERROR;
                }
            }

            memcpy(tfs->buffer_bat, buffer, datasize - written);
            written = datasize;
        } else {
            memcpy(tfs->buffer_bat, buffer, TFS_BLOCK_SIZE);
            written += TFS_BLOCK_SIZE;
            buffer = (void *)((uintptr_t)buffer + TFS_BLOCK_SIZE);
        }

        req.start = tfs->startblock + __builtin_bswap32(tfs->buffer_inode->block[b1]);
        r = tfs->disk->write_block(tfs->disk, &req);
        if(r == 0) {
            return VFS_ERROR;
        }

        b1++;
    }

    return written;
}

int tfs_getfree(fs_t* fs) {
    tfs_t *tfs = (tfs_t *)fs->internal;
    gbd_request_t req;
    req.start = tfs->startblock + TFS_ALLOCATION_BLOCK;
    req.count = 1;
    req.buf = tfs->buffer_bat;
    int r = tfs->disk->read_block(tfs->disk, &req);
    if(r == 0) {
        return VFS_ERROR;
    }

    int allocated = 0;
    swap32_multi(tfs->buffer_bat, tfs->totalblocks / 8 / 4);
    Bitmap bat(tfs->totalblocks / 8, tfs->buffer_bat);
    for(int i = 0; i < tfs->totalblocks; i++) {
        if(bat[i]) {
            allocated++;
        }
    }

    return (tfs->totalblocks - allocated) * TFS_BLOCK_SIZE;
}

int tfs_filecount(fs_t* fs, const char* dirname) {
    tfs_t *tfs = (tfs_t *)fs->internal;
    gbd_request_t req;
    if(strncmp("/", dirname, 1) != 0) {
        return VFS_NOT_FOUND;
    }

    req.start = tfs->startblock + TFS_DIRECTORY_BLOCK;
    req.count = 1;
    req.buf = tfs->buffer_md;
    int r = tfs->disk->read_block(tfs->disk, &req);
    if(r == 0) {
        return VFS_ERROR;
    }

    int count = 0;
    for(int i = 0; i < TFS_MAX_FILES; i++) {
        if(tfs->buffer_md[i].inode != 0) {
            count++;
        }
    }

    return count;
}

int tfs_file(fs_t* fs, const char* dirname, int idx, char* buffer) {
    tfs_t *tfs = (tfs_t *)fs->internal;
    gbd_request_t req;
    if(idx < 0 || strncmp("/", dirname, 1) != 0) {
        return VFS_NOT_FOUND;
    }

    req.start = tfs->startblock + TFS_DIRECTORY_BLOCK;
    req.count = 1;
    req.buf = tfs->buffer_md;
    int r = tfs->disk->read_block(tfs->disk, &req);
    if(r == 0) {
        return VFS_ERROR;
    }

    int count = 0;
    for(int i = 0; i < TFS_MAX_FILES; i++) {
        if(tfs->buffer_md[i].inode != 0 && count++ == idx) {
            strncpy(buffer, tfs->buffer_md[i].name, TFS_MAX_FILESIZE);
            return VFS_OK;
        }
    }

    return VFS_ERROR;
}

int tfs_filesize(fs_t* fs, int fileid) {
    tfs_t *tfs = (tfs_t *)fs->internal;
    if(fileid < 2 || fileid > (int)tfs->totalblocks) {
        return VFS_ERROR;
    }

    gbd_request_t req;
    req.start = tfs->startblock + fileid;
    req.count = 1;
    req.operation = GBD_OPERATION_READ;
    req.buf = tfs->buffer_inode;
    int r = tfs->disk->read_block(tfs->disk, &req);
    if(r == 0) {
        return VFS_ERROR;
    }

    return __builtin_bswap32(tfs->buffer_inode->filesize);
}

// This function is used to check if a TFS exists on a disk at the given location
fs_t* tfs_init(gbd_t* disk, uint32_t sector) {
    if(disk->block_size(disk) != TFS_BLOCK_SIZE) {
        // Sanity, we need 512-byte blocks
        return nullptr;
    }

    void* addr = PageFrameAllocator::SharedAllocator()->RequestPage();
    if(!addr) {
        uart_print("!! tfs_init: Could not allocate memory\r\n");
        return nullptr;
    }

    KERNEL_ASSERT(4096 >= (3*TFS_BLOCK_SIZE+sizeof(tfs_t)+sizeof(fs_t)));

    gbd_request_t req = create_read_block_req(sector + TFS_HEADER_BLOCK, addr);
    int r = disk->read_block(disk, &req);
    if(r == 0) {
        PageFrameAllocator::SharedAllocator()->FreePage(addr);
        uart_print("!! tfs_init: Error during disk read.  Intialization Failed\r\n");
        return nullptr;
    }

    // The filesystem is marked with a 4 byte magic, which only occupies the lower 2 bytes
    uint16_t magic = (uint16_t)__builtin_bswap32(*(uint32_t *)addr);
    if(magic != TFS_MAGIC) {
         PageFrameAllocator::SharedAllocator()->FreePage(addr);
         return nullptr;
    }

    char name[TFS_VOLNAME_MAX];
    strncpy(name, (char *)addr + 4, TFS_VOLNAME_MAX);

    fs_t* fs = (fs_t *)addr;
    tfs_t* tfs = (tfs_t *)((uint8_t *)addr + sizeof(fs_t));
    tfs->buffer_inode = (tfs_inode_t *)((uintptr_t)tfs + sizeof(tfs_t));
    tfs->buffer_bat = (uint32_t *) ((uintptr_t)tfs->buffer_inode + TFS_BLOCK_SIZE);
    tfs->buffer_md = (tfs_direntry_t *)((uintptr_t)tfs->buffer_bat + TFS_BLOCK_SIZE);

    tfs->startblock = sector;
    tfs->totalblocks = disk->total_blocks(disk);
    if(tfs->totalblocks > 8 * TFS_BLOCK_SIZE) {
        tfs->totalblocks = 8 * TFS_BLOCK_SIZE;
    }

    tfs->disk = disk;
    fs->internal = tfs;
    strncpy(fs->volume_name, name, VFS_NAME_LENGTH);

    fs->unmount = tfs_unmount;
    fs->open = tfs_open;
    fs->close = tfs_close;
    fs->create  = tfs_create;
    fs->remove  = tfs_remove;
    fs->read    = tfs_read;
    fs->write   = tfs_write;
    fs->getfree  = tfs_getfree;
    fs->filecount = tfs_filecount;
    fs->file      = tfs_file;
    fs->filesize = tfs_filesize;

    return fs;
}