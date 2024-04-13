#include <main.h>


struct udp_handler{
    void (*handler)(void*, net_device_t*, struct udphdr*, size_t);
    void* external_data;
    uint64_t index;
};


struct udp_port_redirection{
    vector_t* handlers;
};

void** udp_redirection_port_first_level = NULL;
size_t udp_redirection_port_level_count = 0;

static void udp_add_port_redirection(uint16_t port, struct udp_port_redirection* redirection_data){
    assert(udp_redirection_port_first_level);

    size_t first_level_index = port / udp_redirection_port_level_count;
    struct udp_port_redirection** redirection_port_second_level = udp_redirection_port_first_level[first_level_index];

    size_t second_level_index = port % udp_redirection_port_level_count;

    if(redirection_port_second_level == NULL){
        redirection_port_second_level = calloc(udp_redirection_port_level_count, sizeof(void*));
        udp_redirection_port_first_level[first_level_index] = redirection_port_second_level;
    }
    
    redirection_port_second_level[second_level_index] = redirection_data;
}

static struct udp_port_redirection* udp_get_port_redirection(uint16_t port){
    assert(udp_redirection_port_first_level);

    size_t first_level_index = port / udp_redirection_port_level_count;
    struct udp_port_redirection** redirection_port_second_level = udp_redirection_port_first_level[first_level_index];

    if(redirection_port_second_level != NULL){
        size_t second_level_index = port % udp_redirection_port_level_count;
        struct udp_port_redirection* redirection_data = redirection_port_second_level[second_level_index];
        return redirection_data;
    }

    return NULL;
}

uint64_t udp_listen_port(uint16_t port, void* handler, void* external_data){
    struct udp_port_redirection* port_redirection = udp_get_port_redirection(port);

    if(port_redirection == NULL){
        port_redirection = malloc(sizeof(struct udp_port_redirection));
        port_redirection->handlers = vector_create();
        udp_add_port_redirection(port, port_redirection);
    }

    struct udp_handler* handler_data = malloc(sizeof(struct udp_handler));

    handler_data->handler = handler;
    handler_data->external_data = external_data;
    handler_data->index = vector_push(port_redirection->handlers, handler_data);

    return handler_data->index;
}

int udp_remove_listen_port(uint16_t port, uint64_t index){
    struct udp_port_redirection* port_redirection = udp_get_port_redirection(port);

    if(port_redirection){
        struct udp_handler* handler_data = vector_get(port_redirection->handlers, index);
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

int init_udp(void){
    udp_redirection_port_level_count = 256;
    udp_redirection_port_first_level = calloc(udp_redirection_port_level_count, sizeof(void*));
    return 0;
}

int process_udp_packet(net_device_t* net_device, uint32_t saddr, size_t size, void* buffer){
    struct udphdr* udp_header = (struct udphdr*)buffer;

    #ifdef NET_DEBUG
    log_info("UDP header >\n\tSource port : %d\n\tDestination port : %d\n\tLength : %d\n\tChecksum : %d\n\tData length : %d\n",
        ntohs(udp_header->uh_sport),
        ntohs(udp_header->uh_dport),
        ntohs(udp_header->uh_ulen),
        ntohs(udp_header->uh_sum),
        size - sizeof(struct udphdr)
    );
    #endif

    struct udp_port_redirection* port_redirection = udp_get_port_redirection(ntohs(udp_header->uh_dport));

    if(port_redirection != NULL){
        for(uint64_t i = 0; i < port_redirection->handlers->length; i++){
            struct udp_handler* handler_data = vector_get(port_redirection->handlers, i);
            handler_data->handler(handler_data->external_data, net_device, udp_header, size);
        }
    }

    return 0;
}

int generate_udp_packet(net_device_t* net_device, uint32_t daddr, uint16_t dport, uint16_t sport, size_t data_size, void* data_buffer){
    net_device_internal_t* internal = (net_device_internal_t*)net_device->external_data;
    
    size_t packet_size = data_size + sizeof(struct udphdr);
    void* packet_buffer = malloc(packet_size);
    void* packet_buffer_data = (void*)((uintptr_t)packet_buffer + (uintptr_t)sizeof(struct udphdr));

    /* Setup data */
    struct udphdr* udp_header = (struct udphdr*)packet_buffer;
    udp_header->uh_sport = sport; // big endian 
    udp_header->uh_dport = dport; // big endian
    udp_header->uh_ulen = htons(packet_size);
    udp_header->uh_sum = 0; // done after

    memcpy(packet_buffer_data, data_buffer, data_size);
    
    int error = generate_ip_packets(net_device, internal->default_tos, internal->default_ttl, IPPROTO_UDP, internal->nic_ip, daddr, packet_size, packet_buffer);
    
    free(packet_buffer);

    return error;
}