#include "filesystems.h"
#include "SmartPage.h"
#include "paging/PageFrameAllocator.h"
#include "graphics/BasicRenderer.h"
#include "fs/tfs.h"
#include "io/serial.h"

typedef struct partition {
    uint8_t active;
    uint8_t start_head;
    uint16_t start_cs;
    uint8_t type;
    uint8_t end_head;
    uint16_t end_cs;
    uint32_t start_sector;
    uint32_t size;
} __attribute__((packed)) partition_t;

typedef struct boot_sector {
    uint8_t bootcode[0x1BE];
    partition_t partitions[4];
    uint16_t bootsig;
} __attribute__((packed)) boot_sector_t;

static filesystems_t s_filesystems[] = {
    {"TFS", &tfs_init},
    {nullptr, nullptr}
};

uint32_t get_partition(gbd_t* disk, gbd_request_t* dreq, uint8_t part_num, 
                       uint8_t* pactive, uint8_t* ptype, uint32_t* psize, uint64_t mbr_sector) {
    if(part_num > 3) {
        return 0xffffffff;
    }

    dreq->start = mbr_sector;
    dreq->count = 1;
    int bytes_read = disk->read_block(disk, dreq);
    if(bytes_read <= 0) {
        return 0xffffffff;
    }

    boot_sector_t* bootsector = (boot_sector_t*)((uintptr_t)dreq->buf);
    if(pactive != nullptr) {
        *pactive = bootsector->partitions[part_num].active;
    }

    if(ptype != nullptr) {
        *ptype = bootsector->partitions[part_num].type;
    }

    if(psize != nullptr) {
        *psize = bootsector->partitions[part_num].size;
    }

    return bootsector->partitions[part_num].start_sector;
}

fs_t* read_extended_partition_table(gbd_t* disk, gbd_request_t* dreq, uint32_t start, uint32_t sector) {
    fs_t* fs;
    for(uint8_t i = 0; i < 4; i++) {
        uint32_t pstart = 0, psize = 0;
        uint8_t pactive = 0, ptype = 0;

        pstart = get_partition(disk, dreq, i, &pactive, &ptype, &psize, (uint64_t)start + sector);
        if(pstart == 0xFFFFFFFF || (pactive != 0x80 && pactive != 0x00)) {
            continue;
        }

        if(ptype == 0x00) {
            break;
        }

        if(ptype == 0x05 || ptype == 0x0F) {
            fs = read_extended_partition_table(disk, dreq, start, pstart);
            break;
        }

        for(filesystems_t* driver = s_filesystems; driver->name != nullptr; driver++) {
            fs = driver->init(disk, (start + sector + pstart));
            if(fs) {
                uart_printf("VFS: %s initialized on disk at 0x%08x at ext partition %u\n", driver->name, 
                    disk->device->io_address, (uint32_t)i);
                return fs;
            }
        }
    }
    
    return nullptr;
}

fs_t* filesystems_try_all(gbd_t* disk) {
    SmartPage addr(1);
    fs_t* fs = nullptr;

    gbd_request_t req;
    req.buf = (void *)addr;
    req.operation = GBD_OPERATION_READ;
    req.start = 0;
    req.count = 1;

    for(int i = 0; i < 4; i++) {
        uint32_t pstart = 0, psize = 0;
        uint8_t pactive = 0, ptype = 0;

        pstart = get_partition(disk, &req, i, &pactive, &ptype, &psize, 0);
        if(pstart == 0xffffffff || (pactive != 0x80 && pactive != 0x00)) {
            continue;
        }

        if(ptype == 0x00) {
            break;
        }

        if(ptype == 0x05 || ptype == 0x0F) {
            fs = read_extended_partition_table(disk, &req, pstart, 0);
            break;
        }
    }

    if(!fs) {
        for(filesystems_t* driver = s_filesystems; driver->name != nullptr; driver++) {
            fs = driver->init(disk, 0);
            if(fs) {
                uart_printf("VFS: %s initialized on disk at 0x%08x\r\n", driver->name, 
                    disk->device->io_address);
                return fs;
            }
        }
    }

    return nullptr;
}