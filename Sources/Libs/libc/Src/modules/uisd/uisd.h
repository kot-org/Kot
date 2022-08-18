#ifndef _UISD_H
#define _UISD_H 1

#include <kot/sys.h>
#include <kot/types.h>
#include <kot/memory.h>
#include <kot/cstring.h>

#define UISDCreateTask      0x0
#define UISDGetTask         0x1
#define UISDFreeTask        0x2
#define UISDMaxController   0xff

#if defined(__cplusplus)
extern "C" {
#endif

static inline KResult CreateIPC(char* Name, thread thread){

}

static inline thread CallIPC(char* Name){

}


enum ControllerTypeEnum{
    ControllerTypeEnum_Graphics     = 0,
    ControllerTypeEnum_Audio        = 1,
    ControllerTypeEnum_Storage      = 2,
    ControllerTypeEnum_VFS          = 3,
    ControllerTypeEnum_USB          = 4,
    ControllerTypeEnum_Internet     = 5,
    ControllerTypeEnum_Other        = 255
};

typedef struct {
    uint64_t Version;
    uint64_t VendorID;
    enum ControllerTypeEnum Type; 
} controller_t;

typedef struct {
    controller_t ControllerHeader;
    thread GetUSBDevice;
    thread SendUSBPacket;
    thread ReceiveUSBPacket;
} usb_t;

typedef struct {
    controller_t ControllerHeader;
    thread GetPCIDevice;
    thread SetupMSIX;
} pci_t;

typedef struct {
    controller_t ControllerHeader;
    thread GetDiskInfo;
    thread GetPartitionInfo;

    thread ReadDisk;
    thread WriteDisk;

    thread GetVendorID;
    thread GetDeviceID;
} storage_t;

typedef struct {
    controller_t ControllerHeader;
    thread rename;
    thread remove;
    thread fopen;
    thread mkdir;
    thread readdir;
    thread flist;
} vfs_t;

#if defined(__cplusplus)
}
#endif

#endif