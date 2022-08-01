#pragma once

#include <kot/sys.h>

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