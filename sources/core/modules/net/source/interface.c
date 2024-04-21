#include <main.h>

net_handler_t net_internal_handler;

vector_t* net_devices;

int add_net_device(net_device_t* net_device){
    net_device->external_data = malloc(sizeof(net_device_internal_t));
    net_device_internal_t* internal = (net_device_internal_t*)net_device->external_data;
    internal->nic_ip = 0;
    internal->default_ttl = 255;
    internal->default_tos = 0;

    int error = get_dhcp_info(net_device);

    internal->index = vector_push(net_devices, net_device);

    return error;
}

int remove_net_device(net_device_t* net_device){
    net_device_internal_t* internal = (net_device_internal_t*)net_device->external_data;
    vector_remove(net_devices, internal->index);

    free(net_device->external_data);

    return 0;
}

net_device_t* get_main_net_device(void){
    return vector_get(net_devices, 0);
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
    net_devices = vector_create();

    net_internal_handler.add_net_device = &add_net_device;
    net_internal_handler.remove_net_device = &remove_net_device;
    net_internal_handler.rx_packet = &rx_packet;
    net_handler = &net_internal_handler;

    return 0;
}