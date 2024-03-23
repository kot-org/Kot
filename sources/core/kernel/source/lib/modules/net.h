#ifndef LIB_MODULES_NET_H
#define LIB_MODULES_NET_H 1

#include <stdint.h>
#include <stddef.h>
#include <net/ethernet.h>

enum packet_type{
    packet_type_unknow,
    packet_type_ethernet,
};

typedef struct net_device_t{
    int (*tx_packet)(struct net_device_t*, size_t, void*);
    uint8_t mac_address[ETHER_ADDR_LEN];
    enum packet_type packet_type;
    void* internal_data;
    void* external_data; // reserved for net handler
} net_device_t;

typedef struct net_handler_t{
    int (*add_net_device)(net_device_t*);
    int (*remove_net_device)(net_device_t*);
    int (*rx_packet)(net_device_t*, size_t, void*);
} net_handler_t;

#endif // LIB_MODULES_NET_H