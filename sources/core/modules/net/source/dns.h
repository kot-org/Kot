#ifndef _MODULE_NET_DNS_H
#define _MODULE_NET_DNS_H

#include <dns_header.h>
#include <global/modules.h>

int init_dns(void);

int dns_resolve_ip(net_device_t* net_device, char* name, uint32_t* address, uint32_t* ttl, uint8_t* error);


#endif // _MODULE_NET_DNS_H    