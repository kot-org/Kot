#include <main.h>

int init_tcp(void){
    return 0;
}

int process_tcp_packet(net_device_t* net_device, size_t size, void* buffer){
    struct tcphdr* tcp_header = (struct tcphdr*)buffer;

    size_t data_offset = tcp_header->th_off * sizeof(uint32_t);
    void* buffer_data = (void*)((uintptr_t)buffer + (uintptr_t)data_offset);
    size_t size_data = size - data_offset;
    
    #ifdef NET_DEBUG
    log_info("TCP header >\n\tSource port : %d\n\tDestination port : %d\n\tSequence number : %u\n\tAcknowledgement number : %u\n\tData offset : %d\n\tReserved : %d\n\tFlags : 0x%02x\n\tWindow : %d\n\tChecksum : %d\n\tUrgent pointer : %d\n\tData length : %d\n",
        __bswap_16(tcp_header->th_sport),
        __bswap_16(tcp_header->th_dport),
        __bswap_32(tcp_header->th_seq),
        __bswap_32(tcp_header->th_ack),
        tcp_header->th_off,
        tcp_header->th_x2,
        tcp_header->th_flags,
        __bswap_16(tcp_header->th_win),
        __bswap_16(tcp_header->th_sum),
        __bswap_16(tcp_header->th_urp),
        size_data
    );
    #endif

    return 0;
}