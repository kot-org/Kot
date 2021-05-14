#pragma once

#ifndef __cplusplus
#error C++ only
#endif

#include "drivers/gbd.h"
#include <cstdint>

/**
 * The Virtual Filesystem!  What this area is responsible for is gathering all of the 
 * known physical filesystems and presenting them together in a way that makes it look
 * like everything is on a single filesystem.  So if you had a RAMFS, FAT32, ext4, etc,
 * they could all be mounted at paths and look like a single cohesive unit
 */

// Returns statuses for the virtual filesystem
constexpr int8_t VFS_OK             = 0;
constexpr int8_t VFS_NOT_SUPPORTED  = -1;
constexpr int8_t VFS_ERROR          = -2;
constexpr int8_t VFS_INVALID_PARAMS = -3;
constexpr int8_t VFS_NOT_OPEN       = -4;
constexpr int8_t VFS_NOT_FOUND      = -5;
constexpr int8_t VFS_NO_SUCH_FS     = -6;
constexpr int8_t VFS_LIMIT          = -7;
constexpr int8_t VFS_IN_USE         = -8;
constexpr int8_t VFS_UNUSABLE       = -9;

// Some arbitrary limits since I don't actually know what I'm doing :D
constexpr uint8_t VFS_NAME_LENGTH = 16;
constexpr uint16_t VFS_PATH_LENGTH = 256;

typedef int openfile_t;

/* 
This is the heart of the logic.  Basically all the primitive functions that you would like
to use on a filesystem (or rather all the ones that I can think of) are here as the C equivalent
of virtual functions.  What they *actually* do is dependent on the filesystem backing this struct.

For a simple example of how this is implemented, see fs/tfs.h
*/
typedef struct fs_struct {
    // Internal stuff, if you look at some of the filesystem implementations
    // you can see how it can be used
    void* internal;

    // A name for this volume, so that it can be identified vs other filesystems
    char volume_name[16];

    // Any actions needed to be taken, such as journaling, upon unmount
    int (*unmount)(fs_struct*);

    // Open a file by path
    int (*open)(fs_struct*, const char*);

    // Close a file by handle
    int (*close)(fs_struct*, int);

    // Read up to X bytes from a file ID, given a buffer, a buffer size, and an offset
    int (*read)(fs_struct*, int, void*, int, int);

    // Write up to X bytes to a file ID, given a buffer, a buffer size, and an offset
    int (*write)(fs_struct*, int, void*, int, int);

    // Create a file given a size (not ideal) and permissions (not yet implemented)
    int (*create)(fs_struct*, const char*, int, int);

    // Remove a file from the filesystem by path
    int (*remove)(fs_struct*, const char*);

    // Get the number of free sectors
    int (*getfree)(fs_struct*);

    // Get the number of files in a given directory
    int (*filecount)(fs_struct*, const char*);

    // Gets the name of the nth file in a directory
    int (*file)(fs_struct*, const char*, int, char *);

    // Gets the size of a file, given its ID
    int (*filesize)(fs_struct*, int);
} fs_t;

// The init function, this must be called once or it is not usable
void vfs_init();

// Searches all the known hard disks and probes them for filesystems
void vfs_mount_all();

// A class for working with a file from the VFS
class VirtualFilesystemFile {
public:
    VirtualFilesystemFile(openfile_t file, bool owner);
    ~VirtualFilesystemFile();

    int seek(int position);
    int read(void* buffer, size_t bufsize);
    int write(void* buffer, size_t dataSize);
    int size();
    
private:
    openfile_t _file;
    bool _owner;
    VirtualFilesystemFile(const VirtualFilesystemFile &other) = delete;
    VirtualFilesystemFile(VirtualFilesystemFile &&other) = delete;
};

// Opens a file by path from the virtual filesystem
// The form of the path is currently [volname]/path/to/file
openfile_t vfs_open(const char* path);

// Create a file at a given path with the given size (path format same as above)
int vfs_create(const char* path, size_t size);

// Remove a file at the given path (path format same as above)
int vfs_remove(const char* path);