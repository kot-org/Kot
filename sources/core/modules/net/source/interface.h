#ifndef _MODULE_NET_INTERFACE_H
#define _MODULE_NET_INTERFACE_H

#include <errno.h>
#include <protocols.h>
#include <global/socket.h>
#include <global/modules.h>

net_device_t* get_main_net_device(void);
int tx_packet(net_device_t* net_device, size_t size, void* buffer);

#endif // _MODULE_NET_INTERFACE_H