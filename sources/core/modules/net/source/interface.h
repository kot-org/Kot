#ifndef _MODULE_NET_INTERFACE_H
#define _MODULE_NET_INTERFACE_H

#include <protocols.h>
#include <global/modules.h>

int tx_packet(net_device_t* net_device, size_t size, void* buffer);

#endif // _MODULE_NET_INTERFACE_H