#ifndef KOT_UISD_H
#define KOT_UISD_H 1

#include <kot/sys.h>
#include <kot/types.h>
#include <kot/keyhole.h>

namespace Kot{
    #define UISDGetTask         0x1
    #define UISDCreateTask      0x0
    #define UISDFreeTask        0x2
    #define UISDMaxController   0xff

    #define UISDServerEntry [[ noreturn ]] 


    #define ControllerCount 0x9

    enum ControllerTypeEnum {
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
        enum ControllerTypeEnum Type;
        kot_process_t Process;
    } uisd_controller_t;

    typedef struct {
        uisd_controller_t ControllerHeader; 
        kot_thread_t LoadExecutable;
        kot_thread_t GetFramebuffer;
        kot_thread_t ReadFileInitrd;
        kot_thread_t GetTableInRootSystemDescription;
        kot_thread_t GetSystemManagementBIOSTable;
        kot_thread_t BindIRQLine;
        kot_thread_t UnbindIRQLine;
        kot_thread_t BindFreeIRQ;
        kot_thread_t UnbindIRQ;
    } uisd_system_t;

    typedef struct {
        uisd_controller_t ControllerHeader;

        kot_thread_t SetTimePointerKey;
        kot_thread_t SetTickPointerKey;

        kot_ksmem_t TimePointerKey;

        uint64_t TickPeriod;
        kot_ksmem_t TickPointerKey;
    } uisd_time_t;

    typedef struct {
        uisd_controller_t ControllerHeader;

        kot_event_t MouseRelative;
        kot_event_t MouseAbsolute;
        kot_event_t KeyboardEvent;
    } uisd_hid_t;

    typedef struct {
        uisd_controller_t ControllerHeader;

        kot_thread_t CreateWindow;
    } uisd_graphics_t;

    typedef struct {
        uisd_controller_t ControllerHeader;

        kot_event_t OnDeviceChanged;

        kot_thread_t RequestStream;
        kot_thread_t ChangeVolume;
        kot_thread_t SetDefault;
        kot_thread_t GetDeviceCount;
        kot_thread_t GetDeviceInfo;

        kot_thread_t AddDevice;
        /* TODO */
    } uisd_audio_t;

    typedef struct {
        uisd_controller_t ControllerHeader;
        
        kot_thread_t AddDevice;
        kot_thread_t RemoveDevice;

        kot_thread_t NotifyOnNewPartitionByGUIDType;

        kot_thread_t VFSLoginApp;

        kot_thread_t NewDev;
    } uisd_storage_t;

    typedef struct {
        uisd_controller_t ControllerHeader;
        kot_thread_t GetUSBDevice;
        kot_thread_t SendUSBPacket;
        kot_thread_t ReceiveUSBPacket;
    } uisd_usb_t;

    typedef struct {
        uisd_controller_t ControllerHeader;
        kot_thread_t CountDevices;
        kot_thread_t FindDevice;
        kot_thread_t GetInfoDevice;
        kot_thread_t GetBARDevice;
        kot_thread_t BindMSI;
        kot_thread_t UnbindMSI;
        kot_thread_t ConfigReadWord;
        kot_thread_t ConfigWriteWord;
    } uisd_pci_t;

    typedef struct {
        uisd_controller_t ControllerHeader;
        bool IsAvailableAsFile;
    } uisd_shell_t;


    typedef struct {
        kot_thread_t Self;
        uint64_t Controller;
        bool AwaitCallback;
        uint64_t Location;
        KResult Status;
    } uisd_callbackInfo_t;

    uisd_callbackInfo_t* GetControllerUISD(enum ControllerTypeEnum Controller, uintptr_t* Location, bool AwaitCallback);
    uisd_callbackInfo_t* CreateControllerUISD(enum ControllerTypeEnum Controller, kot_ksmem_t MemoryField, bool AwaitCallback);

    kot_thread_t MakeShareableThread(kot_thread_t Thread, enum Priviledge priviledgeRequired);
    kot_thread_t MakeShareableThreadUISDOnly(kot_thread_t Thread);
    kot_thread_t MakeShareableThreadToProcess(kot_thread_t Thread, kot_process_t Process);
    kot_thread_t MakeShareableSpreadThreadToProcess(kot_thread_t Thread, kot_process_t Process);
    kot_process_t ShareProcessKey(kot_process_t Process);

    uintptr_t GetControllerLocationUISD(enum ControllerTypeEnum Controller);
    uintptr_t FindControllerUISD(enum ControllerTypeEnum Controller);
}

#endif