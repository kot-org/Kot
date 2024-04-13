#ifndef _MODULE_NET_MAIN_H
#define _MODULE_NET_MAIN_H

#include <ip.h>
#include <arp.h>
#include <dns.h>
#include <tcp.h>
#include <udp.h>
#include <dhcp.h>
#include <lib/net.h>
#include <lib/log.h>
#include <lib/time.h>
#include <byteswap.h>
#include <ethernet.h>
#include <interface.h>
#include <protocols.h>
#include <lib/vector.h>
#include <lib/assert.h>
#include <lib/memory.h>
#include <lib/string.h>
#include <global/heap.h>
#include <global/modules.h>

#define NET_DEBUG

typedef struct{
    uint32_t nic_ip;
    uint32_t dns_ip;
    uint32_t router_ip;
    uint32_t subnet_mask_ip;
    uint8_t default_ttl;
    uint8_t default_tos;
}net_device_internal_t;


#endif // _MODULE_NET_MAIN_H
