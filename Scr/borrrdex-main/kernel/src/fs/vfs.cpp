#include "vfs.h"
#include "filesystems.h"
#include "config.h"
#include "graphics/BasicRenderer.h"
#include "stdatomic.h"
#include "Panic.h"
#include "io/serial.h"
#include "string.h"
#include <utility>

struct vfs_entry {
    fs_t* filesystem;

    char mountpoint[VFS_NAME_LENGTH];
};

struct openfile_entry {
    fs_t* filesystem;
    int fileid;
    int seek_position;
    int size;
};

static vfs_entry vfs_list[CONFIG_MAX_FILESYSTEMS];
static openfile_entry openfile_list[CONFIG_MAX_OPEN_FILES];
static int vfs_ops;
static int vfs_usable;

static int vfs_start_op() {
    int ret = VFS_OK;

    if(vfs_usable) {
        vfs_ops++;
    } else {
        ret = VFS_UNUSABLE;
    }

    return ret;
}

static void vfs_end_op() {
    vfs_ops--;
    KERNEL_ASSERT(vfs_ops >= 0);
}

class VfsOperation {
public:
    VfsOperation() {
        _isUsable = vfs_start_op() == VFS_OK;
    }

    ~VfsOperation() {
        if(_isUsable) {
            vfs_end_op();
        }
    }

    bool isUsable() const { return _isUsable; }
private:
    bool _isUsable;
};

static int vfs_parse_pathname(const char* pathname, char* volumebuf, char* filenamebuf) {
    if(pathname[0] == '[') {
        pathname++;
        int i;
        for(i = 0; i < VFS_NAME_LENGTH; i++) {
            if(*pathname == 0) {
                return VFS_INVALID_PARAMS;
            }

            if(*pathname == ']') {
                pathname++;
                break;
            }

            *volumebuf = *pathname;
            volumebuf++;
            pathname++;
        }

        if(i >= VFS_NAME_LENGTH - 1) {
            return VFS_INVALID_PARAMS;
        }
    }

    *volumebuf = 0;

    for(int i = 0; i < VFS_NAME_LENGTH; i++) {
        *filenamebuf = *pathname;
        if(*pathname == 0) {
            if(i == 0) {
                return VFS_INVALID_PARAMS;
            }

            *filenamebuf = 0;
            return VFS_OK;
        }

        pathname++;
        filenamebuf++;
    }

    return VFS_INVALID_PARAMS;
}

static int vfs_mount(fs_t *fs, char *name) {
    KERNEL_ASSERT(name && name[0] != 0);

    VfsOperation op;
    if(!op.isUsable()) {
        return VFS_UNUSABLE;
    }

    int i;
    for(i = 0; i < CONFIG_MAX_FILESYSTEMS; i++) {
        if(vfs_list[i].filesystem == nullptr) {
            break;
        }
    }

    int row = i;

    if(row >= CONFIG_MAX_FILESYSTEMS) {
        return VFS_LIMIT;
    }

    for(i = 0; i < CONFIG_MAX_FILESYSTEMS; i++) {
        if(strncmp(vfs_list[i].mountpoint, name, VFS_NAME_LENGTH) == 0) {
            return VFS_ERROR;
        }
    }

    strncpy(vfs_list[row].mountpoint, name, VFS_NAME_LENGTH);
    vfs_list[row].filesystem = fs;
    return VFS_OK;
}

static fs_t* vfs_get_filesystem(const char* mountpoint) {
    for(int row = 0; row < CONFIG_MAX_FILESYSTEMS; row++) {
        if(strncmp(vfs_list[row].mountpoint, mountpoint, VFS_NAME_LENGTH) == 0) {
            return vfs_list[row].filesystem;
        }
    }

    return nullptr;
}

void vfs_init() {
    for(int i = 0; i < CONFIG_MAX_FILESYSTEMS; i++) {
        vfs_list[i].filesystem = nullptr;
    }

    for(int i = 0; i < CONFIG_MAX_OPEN_FILES; i++) {
        openfile_list[i].filesystem = nullptr;
    }

    vfs_ops = 0;
    vfs_usable = true;
    uart_printf("VFS: Max filesystems: %d, Max open files: %d\r\n", CONFIG_MAX_FILESYSTEMS, CONFIG_MAX_OPEN_FILES);
}

int vfs_mount_fs(gbd_t* disk, char* volumename) {
    fs_t* filesystem = filesystems_try_all(disk);
    if(!filesystem) {
        uart_printf("VFS: No filesystem was found on block device 0x%08x\r\n", disk->device->io_address);
        return VFS_NO_SUCH_FS;
    }

    if(!volumename) {
        volumename = filesystem->volume_name;
    }

    if(volumename[0] == 0) {
        uart_printf("VFS: Unknown filesystem volume name, skipping mounting\r\n");
        filesystem->unmount(filesystem);
        return VFS_INVALID_PARAMS;
    }

    int ret;
    if((ret = vfs_mount(filesystem, volumename)) == VFS_OK) {
        uart_printf("VFS: Mounted filesystem volume [%s]\r\n", volumename);
    } else {
        uart_printf("VFS: Mounting of volume [%s] failed\r\n", volumename);
    }

    return ret;
}

void vfs_mount_all() {
    for(int i = 0; i < CONFIG_MAX_FILESYSTEMS; i++) {
        device_t* dev = device_get(TYPECODE_DISK, i);
        if(!dev) {
            return;
        }

        gbd_t* gbd = (gbd_t *)dev->generic_device;
        if(!gbd) {
            continue;
        }

        vfs_mount_fs(gbd, nullptr);
    }
}

openfile_t vfs_open(const char* path) {
    char volumename[VFS_NAME_LENGTH];
    char filename[VFS_NAME_LENGTH];

    VfsOperation op;
    if(!op.isUsable()) {
        return VFS_UNUSABLE;
    }

    if(vfs_parse_pathname(path, volumename, filename) != VFS_OK) {
        return VFS_INVALID_PARAMS;
    }

    openfile_t file;
    for(file = 0; file < CONFIG_MAX_OPEN_FILES; file++) {
        if(openfile_list[file].filesystem == nullptr) {
            break;
        }
    }

    if(file >= CONFIG_MAX_OPEN_FILES) {
        uart_print("!! VFS: Warning, maximum number of open files exceeded.\r\n");
        return VFS_LIMIT;
    }

    fs_t* fs = vfs_get_filesystem(volumename);
    if(!fs) {
        return VFS_NO_SUCH_FS;
    }

    openfile_list[file].filesystem = fs;
    int fileid = fs->open(fs, filename);
    if(fileid < 0) {
        openfile_list[file].filesystem = nullptr;
        return fileid;
    }

    openfile_list[file].fileid = fileid;
    openfile_list[file].seek_position = 0;
    openfile_list[file].size = fs->filesize(fs, fileid);

    return file;
}

int vfs_create(const char* pathname, size_t size) {
    char volumename[VFS_NAME_LENGTH];
    char filename[VFS_NAME_LENGTH];

    VfsOperation op;
    if(!op.isUsable()) {
        return VFS_UNUSABLE;
    }

    if(vfs_parse_pathname(pathname, volumename, filename) != VFS_OK) {
        return VFS_INVALID_PARAMS;
    }

    fs_t* fs = vfs_get_filesystem(volumename);
    if(!fs) {
        return VFS_NO_SUCH_FS;
    }

    return fs->create(fs, filename, size, 0);
}

int vfs_remove(const char* path) {
    char volumename[VFS_NAME_LENGTH];
    char filename[VFS_NAME_LENGTH];

    VfsOperation op;
    if(!op.isUsable()) {
        return VFS_UNUSABLE;
    }

    if(vfs_parse_pathname(path, volumename, filename) != VFS_OK) {
        return VFS_INVALID_PARAMS;
    }

    fs_t* fs = vfs_get_filesystem(volumename);
    if(!fs) {
        return VFS_NO_SUCH_FS;
    }

    return fs->remove(fs, filename);
}

static openfile_entry* vfs_verify_open(openfile_t file) {
    if(file < 0 || file >= CONFIG_MAX_OPEN_FILES) {
        return nullptr;
    }

    openfile_entry* openfile = &openfile_list[file];
    return openfile->filesystem ? openfile : nullptr;
}

VirtualFilesystemFile::VirtualFilesystemFile(openfile_t file, bool owner)
    :_file(file)
    ,_owner(owner)
{
    KERNEL_ASSERT(_file >= 0);
}

VirtualFilesystemFile::~VirtualFilesystemFile() {
    if(!_owner) {
        return;
    }
    
    VfsOperation op;
    if(!op.isUsable()) {
        return;
    }

    openfile_entry* openfile = vfs_verify_open(_file);
    if(!openfile) {
        return;
    }

    fs_t* fs = openfile->filesystem;
    int ret = fs->close(fs, openfile->fileid);
    openfile->filesystem = nullptr;
}

int VirtualFilesystemFile::seek(int position) {
    VfsOperation op;
    if(!op.isUsable()) {
        return VFS_UNUSABLE;
    }

    if(position < 0) {
        return VFS_INVALID_PARAMS;
    }

    openfile_entry* openfile = vfs_verify_open(_file);
    if(!openfile) {
        return VFS_INVALID_PARAMS;
    }

    openfile->seek_position = position;
    return VFS_OK;
}

int VirtualFilesystemFile::read(void* buffer, size_t bufsize) {
    VfsOperation op;
    if(!op.isUsable()) {
        return VFS_UNUSABLE;
    }

    openfile_entry* openfile = vfs_verify_open(_file);
    if(!openfile) {
        return VFS_INVALID_PARAMS;
    }

    fs_t *fs = openfile->filesystem;
    KERNEL_ASSERT(bufsize >= 0 && buffer);

    int ret = fs->read(fs, openfile->fileid, buffer, bufsize, openfile->seek_position);
    if(ret > 0) {
        openfile->seek_position += ret;
    }

    return ret;
}

int VirtualFilesystemFile::write(void* buffer, size_t datasize) {
    VfsOperation op;
    if(!op.isUsable()) {
        return VFS_UNUSABLE;
    }

    openfile_entry* openfile = vfs_verify_open(_file);
    if(!openfile) {
        return VFS_INVALID_PARAMS;
    }

    fs_t* fs = openfile->filesystem;
    KERNEL_ASSERT(datasize >= 0 && buffer);

    int ret = fs->write(fs, openfile->fileid, buffer, datasize, openfile->seek_position);
    if(ret > 0) {
        openfile->seek_position += ret;
    }

    return ret;
}

int VirtualFilesystemFile::size() {
    VfsOperation op;
    if(!op.isUsable()) {
        return VFS_UNUSABLE;
    }

    openfile_entry* openfile = vfs_verify_open(_file);
    if(!openfile) {
        return VFS_INVALID_PARAMS;
    }

    return openfile->size;
}