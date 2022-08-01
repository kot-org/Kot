#pragma once

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
    kthread_t GetUSBDevice;
    kthread_t SendUSBPacket;
    kthread_t ReceiveUSBPacket;
} usb_t;

typedef struct {
    kthread_t GetPCIDevice;
    kthread_t SetupMSIX;
} pci_t;

typedef struct {
    kthread_t GetDiskInfo;
    kthread_t GetPartitionInfo;

    kthread_t ReadDisk;
    kthread_t WriteDisk;

    kthread_t GetVendorID;
    kthread_t GetDeviceID;
} storage_t;

typedef struct {
    kthread_t rename;
    kthread_t remove;
    kthread_t fopen;
    kthread_t mkdir;
    kthread_t readdir;
    kthread_t flist;
} vfs_t;