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
    bool IsReadWrite;
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
    thread_t GetDiskInfo;
    thread_t GetPartitionInfo;

    thread_t ReadDisk;
    thread_t WriteDisk;

    thread_t GetVendorID;
    thread_t GetDeviceID;
} storage_t;

typedef struct {
    controller_t ControllerHeader;
    thread_t rename;
    thread_t remove;
    thread_t fopen;
    thread_t mkdir;
    thread_t readdir;
    thread_t flist;
} vfs_t;

typedef struct {
    controller_t ControllerHeader;
    thread_t GetUSBDevice;
    thread_t SendUSBPacket;
    thread_t ReceiveUSBPacket;
} usb_t;

typedef struct {
    controller_t ControllerHeader;
    thread_t GetBARNum;
    thread_t GetBARType;
    thread_t GetBARSize;
    thread_t PCISearcherGetDevice;
    thread_t PCISearcher;
} pci_t;

typedef struct {
    thread_t Self;
    bool AwaitCallback;
    uintptr_t Location;
    KResult Statu;
} callbackInfo_t;

callbackInfo_t* GetControllerUISD(enum ControllerTypeEnum Controller, uintptr_t* Location, bool AwaitCallback);
callbackInfo_t* CreateControllerUISD(enum ControllerTypeEnum Controller, ksmem_t MemoryField, bool AwaitCallback);

#if defined(__cplusplus)
}
#endif

#endif