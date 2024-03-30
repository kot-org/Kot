#include <main.h>

#define ETHERNET_MINIMUM_SIZE (sizeof(struct ether_header) + sizeof(uint32_t)) // header + crc 32

int init_ethernet(void){
    return 0;
}

int process_ethernet_packet(net_device_t* net_device, size_t size, void* buffer){
    if(size < ETHERNET_MINIMUM_SIZE){
        return -1;
    }

    struct ether_header* ethernet_header = (struct ether_header*)buffer;
    ethernet_header->ether_type = __bswap_16(ethernet_header->ether_type); // Switch big endian to little endian
    
    #ifdef NET_DEBUG
    log_info("Ethernet header >\n\t- Mac source : %02x:%02x:%02x:%02x:%02x:%02x\n\t- Mac destination : %02x:%02x:%02x:%02x:%02x:%02x\n\t- Type : 0x%04x\n", 
    ethernet_header->ether_shost[0], ethernet_header->ether_shost[1], ethernet_header->ether_shost[2], ethernet_header->ether_shost[3], ethernet_header->ether_shost[4], ethernet_header->ether_shost[5],
    ethernet_header->ether_dhost[0], ethernet_header->ether_dhost[1], ethernet_header->ether_dhost[2], ethernet_header->ether_dhost[3], ethernet_header->ether_dhost[4], ethernet_header->ether_dhost[5],
    ethernet_header->ether_type);
    #endif 

    void* buffer_payload = (void*)((uintptr_t)buffer + (uintptr_t)sizeof(struct ether_header));
    size_t size_payload = size - ETHERNET_MINIMUM_SIZE;

    switch (ethernet_header->ether_type){
        case ETHERTYPE_ARP:
            return process_arp_packet(net_device, size_payload, buffer_payload);
        case ETHERTYPE_IP:
            return process_ip_packet(net_device, size_payload, buffer_payload);
        default:
            return -1;
    }
}

int generate_ethernet_packet(net_device_t* net_device, uint8_t* mac_target, uint16_t type, size_t payload_size, size_t* packet_size, void** buffer_ethernet, void** buffer_payload){
    assert(buffer_ethernet);
    assert(buffer_payload);
    assert(packet_size);

    *packet_size = ETHERNET_MINIMUM_SIZE + payload_size;

    *buffer_ethernet = malloc(ETHERNET_MINIMUM_SIZE + payload_size);

    struct ether_header* ethernet_header = (struct ether_header*)*buffer_ethernet;

    if(mac_target){
        memcpy(ethernet_header->ether_dhost, mac_target, ETHER_ADDR_LEN);
    }else{
        memset(ethernet_header->ether_dhost, 0xff, ETHER_ADDR_LEN);
    }

    memcpy(ethernet_header->ether_shost, net_device->mac_address, ETHER_ADDR_LEN);
    ethernet_header->ether_type = __bswap_16(type);

    *buffer_payload = (void*)((uintptr_t)(*buffer_ethernet) + (uintptr_t)sizeof(struct ether_header));

    return 0;
}

int send_ethernet_packet(net_device_t* net_device, uint8_t* mac_target, uint16_t type, size_t payload_size, void* buffer){
    void* ethernet_buffer;
    void* payload_buffer;
    size_t packet_size;

    int error = generate_ethernet_packet(net_device, mac_target, type, payload_size, &packet_size, &ethernet_buffer, &payload_buffer);
    
    if(error){
        return error;
    }

    memcpy(payload_buffer, buffer, payload_size);

    error = tx_packet(net_device, packet_size, ethernet_buffer);

    free(ethernet_buffer);

    return error;
}