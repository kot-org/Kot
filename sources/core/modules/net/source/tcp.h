#ifndef _MODULE_NET_TCP_H
#define _MODULE_NET_TCP_H

#include <protocols.h>
#include <global/modules.h>

int tcp_remove_listen_port(uint16_t port, uint64_t index);
uint64_t tcp_listen_port(uint16_t port, void* handler, void* external_data);
int process_tcp_packet(net_device_t* net_device, uint32_t saddr, size_t size, void* buffer);
int generate_tcp_packet(net_device_t* net_device, uint32_t daddr, uint16_t dport, uint16_t sport, tcp_seq seq, tcp_seq ack, uint8_t off, uint8_t flags, uint16_t win, uint16_t urp, size_t data_size, void* data_buffer);

#endif // _MODULE_NET_TCP_H