#ifndef KOT_UISD_H
#define KOT_UISD_H 1

#include <kot/sys.h>
#include <kot/types.h>
#include <kot/keyhole.h>

#if defined(__cplusplus)
extern "C" {
#endif

extern kot_thread_t kot_srv_system_callback_thread;
extern kot_thread_t kot_srv_storage_callback_thread;
extern kot_thread_t kot_srv_time_callback_thread;
extern kot_thread_t kot_srv_pci_callback_thread;
extern kot_thread_t kot_srv_graphics_callback_thread;
extern kot_thread_t kot_srv_audio_callback_thread;

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
    kot_process_t Process;
} kot_uisd_controller_t;

typedef struct {
    kot_uisd_controller_t ControllerHeader; 
    kot_thread_t LoadExecutable;
    kot_thread_t GetFramebuffer;
    kot_thread_t ReadFileInitrd;
    kot_thread_t GetTableInRootSystemDescription;
    kot_thread_t GetSystemManagementBIOSTable;
    kot_thread_t BindIRQLine;
    kot_thread_t UnbindIRQLine;
    kot_thread_t BindFreeIRQ;
    kot_thread_t UnbindIRQ;
} kot_uisd_system_t;

typedef struct {
    kot_uisd_controller_t ControllerHeader;

    kot_thread_t SetTimePointerKey;
    kot_thread_t SetTickPointerKey;

    kot_key_mem_t TimePointerKey;

    uint64_t TickPeriod;
    kot_key_mem_t TickPointerKey;
} kot_uisd_time_t;

typedef struct {
    kot_uisd_controller_t ControllerHeader;

    kot_event_t MouseRelative;
    kot_event_t MouseAbsolute;
    kot_event_t KeyboardEvent;
} kot_uisd_hid_t;

typedef struct {
    kot_uisd_controller_t ControllerHeader;

    kot_thread_t CreateWindow;
} kot_uisd_graphics_t;

typedef struct {
    kot_uisd_controller_t ControllerHeader;

    kot_event_t OnDeviceChanged;

    kot_thread_t RequestStream;
    kot_thread_t ChangeVolume;
    kot_thread_t SetDefault;
    kot_thread_t GetDeviceCount;
    kot_thread_t GetDeviceInfo;

    kot_thread_t AddDevice;
    /* TODO */
} kot_uisd_audio_t;

typedef struct {
    kot_uisd_controller_t ControllerHeader;
    
    kot_thread_t AddDevice;
    kot_thread_t RemoveDevice;

    kot_thread_t NotifyOnNewPartitionByGUIDType;

    kot_thread_t VFSLoginApp;

    kot_thread_t NewDev;
} kot_uisd_storage_t;

typedef struct {
    kot_uisd_controller_t ControllerHeader;
    kot_thread_t GetUSBDevice;
    kot_thread_t SendUSBPacket;
    kot_thread_t ReceiveUSBPacket;
} kot_uisd_usb_t;

typedef struct {
    kot_uisd_controller_t ControllerHeader;
    kot_thread_t CountDevices;
    kot_thread_t FindDevice;
    kot_thread_t GetInfoDevice;
    kot_thread_t GetBARDevice;
    kot_thread_t BindMSI;
    kot_thread_t UnbindMSI;
    kot_thread_t ConfigReadWord;
    kot_thread_t ConfigWriteWord;
} kot_uisd_pci_t;

typedef struct {
    kot_uisd_controller_t ControllerHeader;
    bool IsAvailableAsFile;
} kot_uisd_shell_t;


typedef struct {
    kot_thread_t Self;
    uint64_t Controller;
    bool AwaitCallback;
    uint64_t Location;
    KResult Status;
} kot_uisd_callbackInfo_t;

kot_uisd_callbackInfo_t* kot_GetControllerUISD(enum kot_uisd_controller_type_enum Controller, uintptr_t* Location, bool AwaitCallback);
kot_uisd_callbackInfo_t* kot_CreateControllerUISD(enum kot_uisd_controller_type_enum Controller, kot_key_mem_t MemoryField, bool AwaitCallback);

kot_thread_t kot_MakeShareableThread(kot_thread_t Thread, enum kot_Priviledge priviledgeRequired);
kot_thread_t kot_MakeShareableThreadUISDOnly(kot_thread_t Thread);
kot_thread_t kot_MakeShareableThreadToProcess(kot_thread_t Thread, kot_process_t Process);
kot_thread_t kot_MakeShareableSpreadThreadToProcess(kot_thread_t Thread, kot_process_t Process);
kot_process_t kot_ShareProcessKey(kot_process_t Process);

uintptr_t kot_GetControllerLocationUISD(enum kot_uisd_controller_type_enum Controller);
uintptr_t kot_FindControllerUISD(enum kot_uisd_controller_type_enum Controller);

KResult kot_ResetUISDThreads();

#if defined(__cplusplus)
} 
#endif

#endif