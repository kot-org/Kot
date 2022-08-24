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

#define ControllerCount 0x7 + 1

enum ControllerTypeEnum{
    ControllerTypeEnum_Test         = 0x0,
    ControllerTypeEnum_System       = 0x1,
    ControllerTypeEnum_Graphics     = 0x2,
    ControllerTypeEnum_Audio        = 0x3,
    ControllerTypeEnum_Storage      = 0x4,
    ControllerTypeEnum_VFS          = 0x5,
    ControllerTypeEnum_USB          = 0x6,
    ControllerTypeEnum_PCI          = 0x7,
    ControllerTypeEnum_Other        = 0xff
};


typedef struct {
    bool IsReadWrite;
    uint64_t Version;
    uint64_t VendorID;
    enum ControllerTypeEnum Type; 
} uisd_controller_t;

typedef struct {
    uisd_controller_t ControllerHeader; 
    /* TODO */
} uisd_system_t;

typedef struct {
    uisd_controller_t ControllerHeader;
    /* TODO */
} uisd_graphics_t;

typedef struct {
    uisd_controller_t ControllerHeader;
    /* TODO */
} uisd_audio_t;

typedef struct {
    uisd_controller_t ControllerHeader;
    thread_t GetDiskInfo;
    thread_t GetPartitionInfo;

    thread_t ReadDisk;
    thread_t WriteDisk;

    thread_t GetVendorID;
    thread_t GetDeviceID;
} uisd_storage_t;

typedef struct {
    uisd_controller_t ControllerHeader;
    thread_t rename;
    thread_t remove;
    thread_t fopen;
    thread_t mkdir;
    thread_t readdir;
    thread_t flist;
} uisd_vfs_t;

typedef struct {
    uisd_controller_t ControllerHeader;
    thread_t GetUSBDevice;
    thread_t SendUSBPacket;
    thread_t ReceiveUSBPacket;
} uisd_usb_t;

typedef struct {
    uisd_controller_t ControllerHeader;
    thread_t GetBARNum;
    thread_t GetBARType;
    thread_t GetBARSize;
    thread_t PCISearcherGetDevice;
    thread_t PCISearcher;
} uisd_pci_t;

typedef struct {
    uisd_controller_t ControllerHeader;
    thread_t GetMemory;
} uisd_test_t;

typedef struct {
    thread_t Self;
    bool AwaitCallback;
    uintptr_t Location;
    KResult Statu;
} uisd_callbackInfo_t;

uisd_callbackInfo_t* GetControllerUISD(enum ControllerTypeEnum Controller, uintptr_t* Location, bool AwaitCallback);
uisd_callbackInfo_t* CreateControllerUISD(enum ControllerTypeEnum Controller, ksmem_t MemoryField, bool AwaitCallback);

#if defined(__cplusplus)
}
#endif

#endif