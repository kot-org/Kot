#ifndef _MODULE_AHCI_DEVICE_H
#define _MODULE_AHCI_DEVICE_H

int ahci_device_read(struct storage_device_t* storage, uint64_t start, size_t size, void* buffer);
int ahci_device_write(struct storage_device_t* storage, uint64_t start, size_t size, void* buffer);

#endif
