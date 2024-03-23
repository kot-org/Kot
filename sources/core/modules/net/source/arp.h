#ifndef _MODULE_NET_ARP_H
#define _MODULE_NET_ARP_H

#include <protocols.h>
#include <global/modules.h>

int send_arp_packet(net_device_t* net_device, uint16_t hrd, uint16_t pro, uint8_t hln, uint8_t pln, uint8_t op, uint8_t* sha, uint8_t* sip, uint8_t* tha, uint8_t* tip);
int generate_arp_header(net_device_t* net_device, uint16_t hrd, uint16_t pro, uint8_t hln, uint8_t pln, uint8_t op, uint8_t* sha, uint8_t* sip, uint8_t* tha, uint8_t* tip, size_t* header_size, struct arphdr** header_buffer);

#endif // _MODULE_NET_ARP_H