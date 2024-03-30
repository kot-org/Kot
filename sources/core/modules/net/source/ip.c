#include <main.h>

vector_t* ip_fragments;
uint16_t ip_last_packet_id;
spinlock_t ip_last_packet_id_lock = (spinlock_t)SPINLOCK_INIT;

typedef struct{
    size_t size;
    void* buffer;
    size_t offset;
}data_ip_fragment_t;


typedef struct{
    /* Identify fragment */
	uint32_t saddr;
	uint32_t daddr;
    uint16_t id;
	uint8_t protocol;

    /* Data fragment */
    vector_t* data;

    size_t size_received;
    ssize_t total_size;

    /* Lock */
    spinlock_t lock;

    uint64_t index;
} ip_fragment_t;

static ip_fragment_t* create_ip_fragment(uint32_t saddr, uint32_t daddr, uint16_t id, uint8_t protocol){
    ip_fragment_t* fragment = (ip_fragment_t*)malloc(sizeof(ip_fragment_t));

    /* There is not need to convert identifier fields to little endian*/
    fragment->saddr = saddr;
    fragment->daddr = daddr;
    fragment->id = id;
    fragment->protocol = protocol;

    fragment->data = vector_create();
    fragment->size_received = 0;
    fragment->total_size = -1;

    fragment->index = vector_push(ip_fragments, fragment);

    return fragment;
}

static int add_ip_fragment(ip_fragment_t* fragment, void* fragment_buffer, size_t fragment_size, size_t fragment_offset, bool is_last_fragment){
    assert(!spinlock_acquire(&fragment->lock));

    if(is_last_fragment){
        fragment->total_size = fragment_offset +  fragment_size;
    }

    fragment->size_received += fragment_size;

    data_ip_fragment_t* data_ip_fragment = (data_ip_fragment_t*)malloc(sizeof(data_ip_fragment_t));
    data_ip_fragment->size = fragment_size;
    data_ip_fragment->offset = fragment_offset;
    data_ip_fragment->buffer = malloc(fragment_size); // we need to copy the buffer because driver don't guarantee when we have return the handler 
    memcpy(data_ip_fragment->buffer, fragment_buffer, fragment_size);

    vector_push(fragment->data, data_ip_fragment);

    spinlock_release(&fragment->lock);

    return 0;
}

static bool is_ip_fragment_entirely_received(ip_fragment_t* fragment){
    return fragment->total_size == fragment->size_received;
}

static int free_ip_fragment(ip_fragment_t* fragment){
    vector_remove(ip_fragments, fragment->index);
    for(uint64_t i = 0; i < fragment->data->length; i++){
        data_ip_fragment_t* data_ip_fragment = vector_get(fragment->data, i);
        free(data_ip_fragment->buffer);
        free(data_ip_fragment);
    }
    vector_clear(fragment->data);
    free(fragment->data);
    free(fragment);
    return 0;
}

static int generate_buffer_ip_fragment(ip_fragment_t* fragment, void** buffer, size_t* size){
    assert(size);
    assert(buffer);
    assert(is_ip_fragment_entirely_received(fragment));

    *buffer = malloc(fragment->total_size);
    *size = fragment->total_size;

    for(uint64_t i = 0; i < fragment->data->length; i++){
        data_ip_fragment_t* data_ip_fragment = vector_get(fragment->data, i);
        void* fragment_address = (void*)(((uintptr_t)*buffer) + (uintptr_t)data_ip_fragment->offset);
        memcpy(fragment_address, data_ip_fragment->buffer, data_ip_fragment->size);    
    }

    return 0;
}

static ip_fragment_t* find_ip_fragment(uint32_t saddr, uint32_t daddr, uint16_t id, uint8_t protocol){
    for(uint64_t i = 0; i < ip_fragments->length; i++){
        ip_fragment_t* fragment = vector_get(ip_fragments, i);
        if(fragment->saddr == saddr && fragment->daddr == daddr && fragment->id == id && fragment->protocol == protocol){
            return fragment;
        }
    }
    return NULL;
}

static uint16_t ip_get_packet_id(void){
    assert(!spinlock_acquire(&ip_last_packet_id_lock));

    uint16_t id = ip_last_packet_id;
    ip_last_packet_id++;

    spinlock_release(&ip_last_packet_id_lock);

    return id;
}

uint16_t ip_checksum(uint8_t* data, size_t len){
    uint32_t result = 0;

    for(; len > 1; data+=2, len-=2){
        result += *(uint16_t*)data;
    }

    if(len){
        uint8_t odd[2] = {*data, 0};
        result += *(uint16_t*)odd;
    }

    while (result>>16)
        result = (result & 0xffff) + (result >> 16);

    return ~result;
}

int init_ip(void){
    ip_fragments = vector_create();
    ip_last_packet_id = 0;
    return 0;
}

int process_full_packet(net_device_t* net_device, struct iphdr* ip_header, size_t size, void* buffer){
    switch (ip_header->protocol) {
        case IPPROTO_TCP:
            return process_tcp_packet(net_device, size, buffer);
        case IPPROTO_UDP:
            return process_udp_packet(net_device, size, buffer);
        default:
            return -1;
    }
}

int process_ip_packet(net_device_t* net_device, size_t size, void* buffer){
    struct iphdr* ip_header = (struct iphdr*)buffer;
    
    #ifdef NET_DEBUG
    log_info("IP header >\n\tVersion : 0x%04x\n\tIHL : 0x%04x\n\tTOS : 0x%02x\n\tLength : %d\n\tIdentification : %d\n\tFragment offset : %d\n\tTTL : %d\n\tProtocol : 0x%02x\n\tChecksum : 0x%04x\n\tSource : %d.%d.%d.%d\n\tDestination : %d.%d.%d.%d\n", 
        ip_header->version,
        ip_header->ihl,
        ip_header->tos,
        __bswap_16(ip_header->tot_len),
        __bswap_16(ip_header->id),
        __bswap_16(ip_header->frag_off),
        ip_header->ttl,
        ip_header->protocol,
        __bswap_16(ip_header->check),
        ((uint8_t*)&ip_header->saddr)[0], ((uint8_t*)&ip_header->saddr)[1],((uint8_t*)&ip_header->saddr)[2],((uint8_t*)&ip_header->saddr)[3],
        ((uint8_t*)&ip_header->daddr)[0], ((uint8_t*)&ip_header->daddr)[1],((uint8_t*)&ip_header->daddr)[2],((uint8_t*)&ip_header->daddr)[3]
    );
    #endif


    if(ip_header->version == IPVERSION){
        if(!(ip_header->frag_off & IP_MF) && (ip_header->frag_off & IP_OFFMASK) == 0){ // there is no fragmentation
            void* buffer_data = (void*)((uintptr_t)buffer + (uintptr_t)((size_t)ip_header->ihl * (size_t)sizeof(uint32_t)));
            size_t size_data = (size_t)__bswap_16(ip_header->tot_len) - (size_t)((size_t)ip_header->ihl * (size_t)sizeof(uint32_t));

            return process_full_packet(net_device, ip_header, size_data, buffer_data);
        }else{
            ip_fragment_t* ip_fragment = find_ip_fragment(ip_header->saddr, ip_header->daddr, ip_header->id, ip_header->protocol);
            if(ip_fragment == NULL){
                ip_fragment = create_ip_fragment(ip_header->saddr, ip_header->daddr, ip_header->id, ip_header->protocol);
            }
            void* buffer_data = (void*)((uintptr_t)buffer + (uintptr_t)((size_t)ip_header->ihl * (size_t)sizeof(uint32_t)));
            size_t size_data = (size_t)__bswap_16(ip_header->tot_len) - (size_t)((size_t)ip_header->ihl * (size_t)sizeof(uint32_t));
            add_ip_fragment(ip_fragment, buffer_data, size_data, ((size_t)(ip_header->frag_off & IP_OFFMASK)) * (size_t)sizeof(uint8_t), ip_header->frag_off & IP_MF);

            if(is_ip_fragment_entirely_received(ip_fragment)){
                void* full_packet_buffer;
                size_t full_packet_size;
                assert(!generate_buffer_ip_fragment(ip_fragment, &full_packet_buffer, &full_packet_size));
                int error = process_full_packet(net_device, ip_header, full_packet_size, full_packet_buffer);
                free(full_packet_buffer);
                assert(!free_ip_fragment(ip_fragment));
                return error;
            }else{
                return 0;
            }
        }
    }else{
        #ifdef NET_DEBUG
        log_warning("IP header not supported !!");
        #endif
        return -1;
    }
}

int generate_ip_packet(net_device_t* net_device, uint8_t ihl, uint8_t tos, uint16_t id, uint16_t frag_off, uint8_t ttl, uint8_t protocol, uint32_t saddr, uint32_t daddr, size_t data_size, void* data_buffer){    
    if(ihl * sizeof(uint32_t) < sizeof(struct iphdr)){
        return -1;
    }

    size_t size = data_size + sizeof(struct iphdr);

    if(net_device->max_size_tx_packet < size){
        return -1;
    }

    void* buffer = malloc(size);
    struct iphdr* ip_header = (struct iphdr*)buffer;
    ip_header->ihl = ihl;
    ip_header->version = IPVERSION;
    ip_header->tos = tos;
    ip_header->tot_len = __bswap_16(size);
    ip_header->id = __bswap_16(id),
    ip_header->frag_off = __bswap_16(frag_off),
    ip_header->ttl = ttl;
    ip_header->protocol = protocol;
    ip_header->saddr = saddr;
    ip_header->daddr = daddr;
    ip_header->check = 0;
    ip_header->check = ip_checksum((uint8_t*)ip_header, sizeof(struct iphdr));

    void* data_buffer_ip = (void*)((uintptr_t)buffer + (uintptr_t)sizeof(struct iphdr));

    memcpy(data_buffer_ip, data_buffer, data_size);

    int error;
    
    switch (net_device->packet_type){
        case packet_type_ethernet:{
            error = send_ethernet_packet(net_device, arp_get_mac_address(net_device, daddr), ETHERTYPE_IP, size, buffer);
            break;
        }
        default:{
            error = -1;
            break;
        }
    }

    free(buffer);

    return error;
}

int generate_ip_packets(net_device_t* net_device, uint8_t tos, uint8_t ttl, uint8_t protocol, uint32_t saddr, uint32_t daddr, size_t data_size, void* data_buffer){
    size_t max_packet_size = net_device->max_size_tx_packet - sizeof(struct iphdr);
    uint8_t ihl = sizeof(struct iphdr) / sizeof(uint32_t);

    void* packet_buffer = data_buffer;
    size_t size_left_to_send = data_size;
    size_t size_send = 0;
    
    uint16_t packet_id = ip_get_packet_id();

    int error = 0;

    for(size_t i = 0; i < data_size; i += max_packet_size){
        size_t packet_size = size_left_to_send;

        if(packet_size > max_packet_size){
            packet_size = max_packet_size;
        }

        size_left_to_send -= packet_size;

        uint16_t frag_off = size_send / sizeof(uint32_t); // 8-bytes block

        if(size_left_to_send > max_packet_size){
            frag_off |= IP_MF;
        }

        error = generate_ip_packet(net_device, ihl, tos, packet_id, frag_off, ttl, protocol, saddr, daddr, packet_size, packet_buffer);
        
        if(error){
            return error;
        }

        packet_buffer = (void*)((uintptr_t)packet_buffer + packet_size);
        size_send += packet_size;
    }

    return 0;
}