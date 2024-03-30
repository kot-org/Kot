#include <main.h>

int init_udp(void){
    return 0;
}

int process_udp_packet(net_device_t* net_device, size_t size, void* buffer){
    struct udphdr* udp_header = (struct udphdr*)buffer;

    size_t data_offset = sizeof(struct udphdr);
    void* buffer_data = (void*)((uintptr_t)buffer + (uintptr_t)data_offset);
    size_t size_data = size - data_offset;

    #ifdef NET_DEBUG
    log_info("UDP header >\n\tSource port : %d\n\tDestination port : %d\n\tLength : %d\n\tChecksum : %d\n\tData length : %d\n",
        __bswap_16(udp_header->uh_sport),
        __bswap_16(udp_header->uh_dport),
        __bswap_16(udp_header->uh_ulen),
        __bswap_16(udp_header->uh_sum),
        size_data
    );
    #endif

    return 0;
}

int generate_udp_packet(net_device_t* net_device, uint32_t daddr, uint16_t dport, uint16_t sport, size_t data_size, void* data_buffer){
    net_device_internal_t* internal = (net_device_internal_t*)net_device->external_data;
    
    size_t packet_size = data_size + sizeof(struct udphdr);
    void* packet_buffer = malloc(packet_size);
    void* packet_buffer_data = (void*)((uintptr_t)packet_buffer + (uintptr_t)sizeof(struct udphdr));

    /* Setup data */
    struct udphdr* udp_header = (struct udphdr*)packet_buffer;
    udp_header->uh_sport = __bswap_16(sport),
    udp_header->uh_dport = __bswap_16(dport),
    udp_header->uh_ulen = __bswap_16(packet_size),
    udp_header->uh_sum = 0; // TODO

    memcpy(packet_buffer_data, data_buffer, data_size);
    
    int error = generate_ip_packets(net_device, internal->default_tos, internal->default_ttl, IPPROTO_UDP, internal->ip, daddr, packet_size, packet_buffer);
    
    free(packet_buffer);

    return error;
}