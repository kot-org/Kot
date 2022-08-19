#ifndef _UISD_H
#define _UISD_H 1

#include <kot/sys.h>
#include <kot/types.h>
#include <kot/memory.h>
#include <kot/cstring.h>
#include <kot/keyhole.h>

#define UISDGetTask         0x1
#define UISDCreateTask      0x0
#define UISDFreeTask        0x2
#define UISDMaxController   0xff

#if defined(__cplusplus)
extern "C" {
#endif

#define ControllerCount 0x5 + 1

enum ControllerTypeEnum{
    ControllerTypeEnum_Graphics     = 0,
    ControllerTypeEnum_Audio        = 1,
    ControllerTypeEnum_Storage      = 2,
    ControllerTypeEnum_VFS          = 3,
    ControllerTypeEnum_USB          = 4,
    ControllerTypeEnum_PCI          = 5,
    ControllerTypeEnum_Other        = 255
};


typedef struct {
    uint64_t Version;
    uint64_t VendorID;
    enum ControllerTypeEnum Type; 
} controller_t;

typedef struct {
    controller_t ControllerHeader;
    /* TODO */
} graphics_t;

typedef struct {
    controller_t ControllerHeader;
    /* TODO */
} audio_t;

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
    thread Self;
    bool AwaitCallback;
    uintptr_t Location;
    KResult Statu;
} callbackInfo_t;

#if defined(__cplusplus)
}
#endif

#endif