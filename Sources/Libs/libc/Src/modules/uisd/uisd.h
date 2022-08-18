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
    parameters_t Parameters;
    Parameters.Arg0 = UISDCreateTask;
    size_t lenght = strlen(Name);
    if(lenght > 8) lenght = 8;
    Parameters.Arg1 = *(uint64_t*)Name;
    Parameters.Arg2 = (uint64_t)thread;

    return Sys_IPC(KotSpecificData.UISDHandler, (parameters_t*)&Parameters, false);
}

static inline thread CallIPC(char* Name){
    parameters_t Parameters;
    Parameters.Arg0 = UISDGetTask;
    size_t lenght = strlen(Name);
    if(lenght > 8) lenght = 8;
    Parameters.Arg1 = *(uint64_t*)Name;
    return Sys_IPC(KotSpecificData.UISDHandler, (parameters_t*)&Parameters, false);
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
    ksmem_t Data;
} controller_t;

typedef struct {
    thread GetUSBDevice;
    thread SendUSBPacket;
    thread ReceiveUSBPacket;
} usb_t;

typedef struct {
    thread GetPCIDevice;
    thread SetupMSIX;
} pci_t;

typedef struct {
    thread GetDiskInfo;
    thread GetPartitionInfo;

    thread ReadDisk;
    thread WriteDisk;

    thread GetVendorID;
    thread GetDeviceID;
} storage_t;

typedef struct {
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