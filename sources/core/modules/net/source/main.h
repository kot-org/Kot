#ifndef _MODULE_NET_MAIN_H
#define _MODULE_NET_MAIN_H

#include <ip.h>
#include <arp.h>
#include <tcp.h>
#include <udp.h>
#include <lib/log.h>
#include <lib/time.h>
#include <byteswap.h>
#include <ethernet.h>
#include <interface.h>
#include <protocols.h>
#include <lib/vector.h>
#include <lib/assert.h>
#include <lib/memory.h>
#include <global/heap.h>
#include <global/modules.h>

#define NET_DEBUG

typedef struct{
    uint32_t ip;
    uint8_t default_ttl;
    uint8_t default_tos;
}net_device_internal_t;


#endif // _MODULE_NET_MAIN_H
