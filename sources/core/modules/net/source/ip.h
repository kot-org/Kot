#ifndef _MODULE_NET_IP_H
#define _MODULE_NET_IP_H

#include <protocols.h>
#include <global/modules.h>

int process_ip_packet(net_device_t* net_device, size_t size, void* buffer);
int generate_ip_packets(net_device_t* net_device, uint8_t tos, uint8_t ttl, uint8_t protocol, uint32_t saddr, uint32_t daddr, size_t data_size, void* data_buffer);
int generate_ip_packet(net_device_t* net_device, uint8_t ihl, uint8_t tos, uint16_t id, uint16_t frag_off, uint8_t ttl, uint8_t protocol, uint32_t saddr, uint32_t daddr, size_t data_size, void* data_buffer);

#endif // _MODULE_NET_IP_H