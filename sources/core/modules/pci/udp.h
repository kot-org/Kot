#ifndef _MODULE_NET_UDP_H
#define _MODULE_NET_UDP_H

#include <protocols.h>
#include <global/modules.h>

int process_udp_packet(net_device_t* net_device, size_t size, void* buffer);

#endif // _MODULE_NET_UDP_H