#include <main.h>

net_handler_t net_internal_handler;

int add_net_device(net_device_t* net_device){
    net_device->external_data = malloc(sizeof(net_device_internal_t));
    net_device_internal_t* internal = (net_device_internal_t*)net_device->external_data;
    internal->ip = 0;
    internal->default_ttl = 255;
    internal->default_tos = 0;

    return get_dhcp_info(net_device);
}

int remove_net_device(net_device_t* net_device){
    return 0;
}

int rx_packet(net_device_t* net_device, size_t size, void* buffer){
    switch (net_device->packet_type){
        case packet_type_ethernet:
            return process_ethernet_packet(net_device, size, buffer);
        default:
            return -1;
    }
}

int tx_packet(net_device_t* net_device, size_t size, void* buffer){
    return net_device->tx_packet(net_device, size, buffer);
}

int interface_init(void){
    net_internal_handler.add_net_device = &add_net_device;
    net_internal_handler.remove_net_device = &remove_net_device;
    net_internal_handler.rx_packet = &rx_packet;
    net_handler = &net_internal_handler;
    return 0;
}