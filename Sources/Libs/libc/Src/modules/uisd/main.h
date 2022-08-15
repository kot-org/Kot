#ifndef _UISD_H
#define _UISD_H 1

#include <kot/sys.h>

typedef enum {
    GRAPHICS    = 0,
    AUDIO       = 1,
    USB         = 2,
    STORAGE     = 3,
    WIFI        = 4,
    BLUETOOTH   = 5,
    RGB         = 6,
    OTHERS      = 255
} DriverTypeEnum;

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

#endif