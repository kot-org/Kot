#ifndef _MODULE_NET_ETHERNET_H
#define _MODULE_NET_ETHERNET_H

#include <protocols.h>
#include <global/modules.h>

int send_ethernet_packet(net_device_t* net_device, uint8_t* mac_target, uint16_t type, size_t payload_size, void* buffer);
int generate_ethernet_packet(net_device_t* net_device, uint8_t* mac_target, uint16_t type, size_t payload_size, size_t* packet_size, void** buffer_ethernet, void** buffer_payload);

#endif // _MODULE_NET_ETHERNET_H