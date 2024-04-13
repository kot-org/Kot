#include <main.h>


struct tcp_handler{
    void (*handler)(void*, net_device_t*, struct tcphdr*, size_t);
    void* external_data;
    uint64_t index;
};


struct tcp_port_redirection{
    vector_t* handlers;
};

void** tcp_redirection_port_first_level = NULL;
size_t tcp_redirection_port_level_count = 0;

static void tcp_add_port_redirection(uint16_t port, struct tcp_port_redirection* redirection_data){
    assert(tcp_redirection_port_first_level);

    size_t first_level_index = port / tcp_redirection_port_level_count;
    struct tcp_port_redirection** redirection_port_second_level = tcp_redirection_port_first_level[first_level_index];

    size_t second_level_index = port % tcp_redirection_port_level_count;

    if(redirection_port_second_level == NULL){
        redirection_port_second_level = calloc(tcp_redirection_port_level_count, sizeof(void*));
        tcp_redirection_port_first_level[first_level_index] = redirection_port_second_level;
    }
    
    redirection_port_second_level[second_level_index] = redirection_data;
}

static struct tcp_port_redirection* tcp_get_port_redirection(uint16_t port){
    assert(tcp_redirection_port_first_level);

    size_t first_level_index = port / tcp_redirection_port_level_count;
    struct tcp_port_redirection** redirection_port_second_level = tcp_redirection_port_first_level[first_level_index];

    if(redirection_port_second_level != NULL){
        size_t second_level_index = port % tcp_redirection_port_level_count;
        struct tcp_port_redirection* redirection_data = redirection_port_second_level[second_level_index];
        return redirection_data;
    }

    return NULL;
}

uint64_t tcp_listen_port(uint16_t port, void* handler, void* external_data){
    struct tcp_port_redirection* port_redirection = tcp_get_port_redirection(port);

    if(port_redirection == NULL){
        port_redirection = malloc(sizeof(struct tcp_port_redirection));
        port_redirection->handlers = vector_create();
        tcp_add_port_redirection(port, port_redirection);
    }

    struct tcp_handler* handler_data = malloc(sizeof(struct tcp_handler));

    handler_data->handler = handler;
    handler_data->external_data = external_data;
    handler_data->index = vector_push(port_redirection->handlers, handler_data);

    return handler_data->index;
}

int tcp_remove_listen_port(uint16_t port, uint64_t index){
    struct tcp_port_redirection* port_redirection = tcp_get_port_redirection(port);

    if(port_redirection){
        struct tcp_handler* handler_data = vector_get(port_redirection->handlers, index);
        if(handler_data){
            vector_remove(port_redirection->handlers, handler_data->index);
            free(handler_data);
            return 0;
        }else{
            return -1;
        }
    }else{
        return -1;
    }
}

int init_tcp(void){
    tcp_redirection_port_level_count = 256;
    tcp_redirection_port_first_level = calloc(tcp_redirection_port_level_count, sizeof(void*));
    return 0;
}

int process_tcp_packet(net_device_t* net_device, uint32_t saddr, size_t size, void* buffer){
    struct tcphdr* tcp_header = (struct tcphdr*)buffer;
    
    #ifdef NET_DEBUG
    size_t data_offset = tcp_header->th_off * sizeof(uint32_t);
    size_t size_data = size - data_offset;
    log_info("TCP header >\n\tSource port : %d\n\tDestination port : %d\n\tSequence number : %u\n\tAcknowledgement number : %u\n\tData offset : %d\n\tReserved : %d\n\tFlags : 0x%02x\n\tWindow : %d\n\tChecksum : %d\n\tUrgent pointer : %d\n\tData length : %d\n",
        ntohs(tcp_header->th_sport),
        ntohs(tcp_header->th_dport),
        ntohl(tcp_header->th_seq),
        ntohl(tcp_header->th_ack),
        tcp_header->th_off,
        tcp_header->th_x2,
        tcp_header->th_flags,
        ntohs(tcp_header->th_win),
        ntohs(tcp_header->th_sum),
        ntohs(tcp_header->th_urp),
        size_data
    );
    #endif

    struct tcp_port_redirection* port_redirection = tcp_get_port_redirection(ntohs(tcp_header->th_sport));

    if(port_redirection != NULL){
        for(uint64_t i = 0; i < port_redirection->handlers->length; i++){
            struct tcp_handler* handler_data = vector_get(port_redirection->handlers, i);
            handler_data->handler(handler_data->external_data, net_device, tcp_header, size);
        }
    }

    return 0;
}

int generate_tcp_packet(net_device_t* net_device, uint32_t daddr, uint16_t dport, uint16_t sport, tcp_seq seq, tcp_seq ack, uint8_t off, uint8_t flags, uint16_t win, uint16_t urp, size_t data_size, void* data_buffer){
    net_device_internal_t* internal = (net_device_internal_t*)net_device->external_data;
    
    size_t packet_size = data_size + sizeof(struct tcphdr);
    void* packet_buffer = malloc(packet_size);
    void* packet_buffer_data = (void*)((uintptr_t)packet_buffer + (uintptr_t)sizeof(struct tcphdr));

    /* Setup data */
    struct tcphdr* tcp_header = (struct tcphdr*)packet_buffer;
    tcp_header->th_sport = sport; // big endian
    tcp_header->th_dport = dport; // big endian
    tcp_header->th_seq = seq; // big endian
    tcp_header->th_ack = ack; // big endian
    tcp_header->th_x2 = 0;
    tcp_header->th_off = off;
    tcp_header->th_flags = flags;
    tcp_header->th_win = win; // big endian
    tcp_header->th_sum = 0; // done done after
    tcp_header->th_urp = urp; // big endian

    memcpy(packet_buffer_data, data_buffer, data_size);
    
    int error = generate_ip_packets(net_device, internal->default_tos, internal->default_ttl, IPPROTO_TCP, internal->nic_ip, daddr, packet_size, packet_buffer);
    
    free(packet_buffer);

    return error;
}