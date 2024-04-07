#ifndef _MODULE_NET_UDP_H
#define _MODULE_NET_UDP_H

#include <protocols.h>
#include <global/modules.h>

int udp_remove_listen_port(uint16_t port, uint64_t index);
uint64_t udp_listen_port(uint16_t port, void* handler, void* external_data);
int process_udp_packet(net_device_t* net_device, uint32_t saddr, size_t size, void* buffer);
int generate_udp_packet(net_device_t* net_device, uint32_t daddr, uint16_t dport, uint16_t sport, size_t data_size, void* data_buffer);

#endif // _MODULE_NET_UDP_H