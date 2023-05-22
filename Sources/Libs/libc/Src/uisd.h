#ifndef _UISD_H
#define _UISD_H 1

#include <kot/sys.h>
#include <kot/types.h>
#include <kot/memory.h>
#include <string.h>
#include <kot/keyhole.h>

#if defined(__cplusplus)
extern "C" {
#endif

extern thread_t srv_system_callback_thread;
extern thread_t srv_storage_callback_thread;
extern thread_t srv_time_callback_thread;
extern thread_t srv_pci_callback_thread;
extern thread_t srv_graphics_callback_thread;
extern thread_t srv_audio_callback_thread;

#define UISDGetTask         0x1
#define UISDCreateTask      0x0
#define UISDFreeTask        0x2
#define UISDMaxController   0xff

#define UISDServerEntry [[ noreturn ]] 

#define ControllerCount 0x9

enum kot_uisd_controller_type_enum {
    ControllerTypeEnum_System       = 0x0,
    ControllerTypeEnum_Time         = 0x1,
    ControllerTypeEnum_Hid          = 0x2,
    ControllerTypeEnum_Graphics     = 0x3,
    ControllerTypeEnum_Storage      = 0x4,
    ControllerTypeEnum_Audio        = 0x5,
    ControllerTypeEnum_USB          = 0x6,
    ControllerTypeEnum_PCI          = 0x7,
    ControllerTypeEnum_Shell        = 0x8,
    ControllerTypeEnum_Other        = 0xff
};


typedef struct {
    bool IsReadWrite;
    uint64_t Version;
    uint64_t VendorID;
    enum kot_uisd_controller_type_enum Type;
    process_t Process;
} uisd_controller_t;

typedef struct {
    uisd_controller_t ControllerHeader; 
    thread_t LoadExecutable;
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

    thread_t SetTimePointerKey;
    thread_t SetTickPointerKey;

    kot_key_mem_t TimePointerKey;

    uint64_t TickPeriod;
    kot_key_mem_t TickPointerKey;
} uisd_time_t;

typedef struct {
    uisd_controller_t ControllerHeader;

    kot_event_t MouseRelative;
    kot_event_t MouseAbsolute;
    kot_event_t KeyboardEvent;
} uisd_hid_t;

typedef struct {
    uisd_controller_t ControllerHeader;

    thread_t CreateWindow;
} uisd_graphics_t;

typedef struct {
    uisd_controller_t ControllerHeader;

    kot_event_t OnDeviceChanged;

    thread_t RequestStream;
    thread_t ChangeVolume;
    thread_t SetDefault;
    thread_t GetDeviceCount;
    thread_t GetDeviceInfo;

    thread_t AddDevice;
} uisd_audio_t;

typedef struct {
    uisd_controller_t ControllerHeader;
    
    thread_t AddDevice;
    thread_t RemoveDevice;

    thread_t NotifyOnNewPartitionByGUIDType;

    thread_t VFSLoginApp;

    thread_t NewDev;
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
    thread_t ConfigReadWord;
    thread_t ConfigWriteWord;
} uisd_pci_t;

typedef struct {
    uisd_controller_t ControllerHeader;
    bool IsAvailableAsFile;
} uisd_shell_t;


typedef struct {
    thread_t Self;
    uint64_t Controller;
    bool AwaitCallback;
    uint64_t Location;
    KResult Status;
} uisd_callbackInfo_t;

uisd_callbackInfo_t* GetControllerUISD(enum kot_uisd_controller_type_enum Controller, void** Location, bool AwaitCallback);
uisd_callbackInfo_t* CreateControllerUISD(enum kot_uisd_controller_type_enum Controller, kot_key_mem_t MemoryField, bool AwaitCallback);

thread_t MakeShareableThread(thread_t Thread, enum Priviledge priviledgeRequired);
thread_t MakeShareableThreadUISDOnly(thread_t Thread);
thread_t MakeShareableThreadToProcess(thread_t Thread, process_t Process);
thread_t MakeShareableSpreadThreadToProcess(thread_t Thread, process_t Process);
process_t ShareProcessKey(process_t Process);

void* GetControllerLocationUISD(enum kot_uisd_controller_type_enum Controller);
void* FindControllerUISD(enum kot_uisd_controller_type_enum Controller);

KResult ResetUISDThreads();

#if defined(__cplusplus)
}
#endif

#endif