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

#define ControllerCount 0x7

enum ControllerTypeEnum {
    ControllerTypeEnum_System       = 0x0,
    ControllerTypeEnum_Graphics     = 0x1,
    ControllerTypeEnum_Storage      = 0x2,
    ControllerTypeEnum_Hid          = 0x3,
    ControllerTypeEnum_Audio        = 0x4,
    ControllerTypeEnum_USB          = 0x5,
    ControllerTypeEnum_PCI          = 0x6,
    ControllerTypeEnum_Other        = 0xff
};


typedef struct {
    bool IsReadWrite;
    uint64_t Version;
    uint64_t VendorID;
    enum ControllerTypeEnum Type;
    process_t Process; 
} uisd_controller_t;

typedef struct {
    uisd_controller_t ControllerHeader; 
    thread_t GetFramebuffer;
    thread_t ReadFileInitrd;
    thread_t GetTableInRootSystemDescription;
    thread_t GetSystemManagementBIOSTable;
    thread_t BindIRQLine;
    thread_t UnbindIRQLine;
    thread_t BindFreeIRQ;
    thread_t UnbindIRQ;
} uisd_system_t;

typedef struct {
    uisd_controller_t ControllerHeader;

    event_t MouseRelative;
    event_t MouseAbsolute;
    event_t KeyboardServer;
    event_t KeyboardClient;
} uisd_hid_t;

typedef struct {

    uisd_controller_t ControllerHeader;

    thread_t create;
    thread_t destroy;
    thread_t getFramebuffer;
    thread_t getWidth;
    thread_t getHeight;
    thread_t getBpp;
    thread_t show;
    thread_t hide;
    thread_t resize;
    thread_t move;
} uisd_graphics_t;

typedef struct {
    uisd_controller_t ControllerHeader;
    /* TODO */
} uisd_audio_t;

typedef struct {
    uisd_controller_t ControllerHeader;
    
    thread_t AddDevice;
    thread_t RemoveDevice;

    thread_t CountPartitionByGUIDTypeSrv;
    thread_t MountPartition;
    thread_t UnmountPartition;

    thread_t Rename;
    thread_t Remove;
    thread_t Fopen;
    thread_t Mkdir;
    thread_t Readdir;
    thread_t Flist;
} uisd_storage_t;

typedef struct {
    uisd_controller_t ControllerHeader;
    thread_t GetUSBDevice;
    thread_t SendUSBPacket;
    thread_t ReceiveUSBPacket;
} uisd_usb_t;

typedef struct {
    uisd_controller_t ControllerHeader;
    thread_t CountDevices;
    thread_t FindDevice;
    thread_t GetInfoDevice;
    thread_t GetBARDevice;
    thread_t BindMSI;
    thread_t UnbindMSI;
} uisd_pci_t;

typedef struct {
    thread_t Self;
    uint64_t Controller;
    bool AwaitCallback;
    uint64_t Location;
    KResult Status;
} uisd_callbackInfo_t;

uisd_callbackInfo_t* GetControllerUISD(enum ControllerTypeEnum Controller, uintptr_t* Location, bool AwaitCallback);
uisd_callbackInfo_t* CreateControllerUISD(enum ControllerTypeEnum Controller, ksmem_t MemoryField, bool AwaitCallback);

thread_t MakeShareableThread(thread_t Thread, enum Priviledge priviledgeRequired);
thread_t MakeShareableThreadUISDOnly(thread_t Thread);
thread_t MakeShareableThreadToProcess(thread_t Thread, process_t Process);
process_t ShareProcessKey(process_t Process);

uintptr_t GetControllerLocationUISD(enum ControllerTypeEnum Controller);
uintptr_t FindControllerUISD(enum ControllerTypeEnum Controller);

#if defined(__cplusplus)
}
#endif

#endif