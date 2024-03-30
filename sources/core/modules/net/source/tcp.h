#ifndef _MODULE_NET_TCP_H
#define _MODULE_NET_TCP_H

#include <protocols.h>
#include <global/modules.h>

int process_tcp_packet(net_device_t* net_device, size_t size, void* buffer);

#endif // _MODULE_NET_TCP_H