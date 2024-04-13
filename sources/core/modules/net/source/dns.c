#include <main.h>

struct dns_request{
    uint32_t address;
    uint32_t ttl;
    uint8_t error;
    bool have_response;
};

vector_t* dns_requests = NULL;

void receive_dns_info(void* external_data, net_device_t* net_device, struct udphdr* udp_header, size_t size){
    void* buffer_data = (void*)((uintptr_t)udp_header + (uintptr_t)sizeof(struct udphdr));
    struct dns_packet* dns_packet = (struct dns_packet*)buffer_data;

    uint32_t ttl = 0;
    uint32_t address = 0;

    if( dns_packet->header.qr == QR_RESPONSE && 
        dns_packet->header.opcode == OPCODE_QUERY &&
        dns_packet->header.tc == 0 &&
        dns_packet->header.rcode == 0 &&
        dns_packet->header.ancount == htons(1)){
            size_t current_index_data = 0;

            /* ignore name */
            if(dns_packet->data[current_index_data] & 0xc0){ /* pointer */
                current_index_data += 2;
            }else{
                while(dns_packet->data[current_index_data] != 0){
                    current_index_data += dns_packet->data[current_index_data] + 1;
                }
                current_index_data++;
            }

            /* ignore question */
            current_index_data += sizeof(uint16_t) * 2;

            /* ignore name */
            if(dns_packet->data[current_index_data] & 0xc0){ /* pointer */
                current_index_data += 2;
            }else{
                while(dns_packet->data[current_index_data] != 0){
                    current_index_data += dns_packet->data[current_index_data] + 1;
                }
            }

            if(*(uint16_t*)&dns_packet->data[current_index_data] == htons(1)){ // Answer is a Type A query (host address)
                current_index_data += sizeof(uint16_t);
                if(*(uint16_t*)&dns_packet->data[current_index_data] == htons(1)){ // Answer is class IN (Internet address)
                    current_index_data += sizeof(uint16_t);
                    ttl = ntohl(*(uint32_t*)&dns_packet->data[current_index_data]); // Response is valid for x seconds
                    current_index_data += sizeof(uint32_t);
                    if(*(uint16_t*)&dns_packet->data[current_index_data] == htons(4)){ // Address is  bytes long
                        current_index_data += sizeof(uint16_t);
                        address = ntohl(*(uint32_t*)&dns_packet->data[current_index_data]);
                    }
                }
            }

    }   
    struct dns_request* request = vector_get(dns_requests, dns_packet->header.id);
    if(request != NULL){
        request->address = address;
        request->ttl = ttl;
        request->error = dns_packet->header.rcode;
        request->have_response = true;
    }
}

int init_dns(void){
    dns_requests = vector_create();

    udp_listen_port(DNS_PORT, &receive_dns_info, NULL);
    return 0;
}

int dns_resolve_ip(net_device_t* net_device, char* name, uint32_t* address, uint32_t* ttl, uint8_t* error){
    assert(dns_requests);

    net_device_internal_t* internal = (net_device_internal_t*)net_device->external_data;

    size_t dot_count = 1;
    size_t name_length = strlen(name);
    for(size_t i = 0; i < name_length; i++){
        if(name[i] == '.'){
            dot_count++;
        }
    }

    volatile struct dns_request request; 
    request.have_response = false;

    uint64_t request_index = vector_push(dns_requests, (void*)&request);
    
    if(request_index > UINT16_MAX){
        vector_remove(dns_requests, request_index);
        return -1;
    }

    size_t dns_packet_size = sizeof(struct dns_packet) + dot_count * sizeof(uint8_t) + name_length + 1 + sizeof(uint16_t) + sizeof(uint16_t);
    struct dns_packet* dns_packet = (struct dns_packet*)malloc(dns_packet_size);
    dns_packet->header.id = request_index;
    dns_packet->header.qr = QR_QUERY;
    dns_packet->header.opcode = OPCODE_QUERY;
    dns_packet->header.aa = AA_NONAUTHORITY;
    dns_packet->header.tc = 0;
    dns_packet->header.rd = 1;
    dns_packet->header.ra = 1;
    dns_packet->header.z = 0;
    dns_packet->header.rcode = 0;
    dns_packet->header.qdcount = htons(1);
    dns_packet->header.ancount = htons(0);
    dns_packet->header.nscount = htons(0);
    dns_packet->header.arcount = htons(0);

    size_t current_index_data = 0;

    char* token = (char*)name;
    while(token != NULL){
        char* next_dot = strchr(token, '.');
        char* current_dot = token;
        if(next_dot){
            *next_dot = '\0';
            token = next_dot + sizeof(char);
        }else{
            token = NULL;
        }

        size_t token_length = strlen(current_dot);
        dns_packet->data[current_index_data] = token_length;
        current_index_data++;
        memcpy(&dns_packet->data[current_index_data], current_dot, token_length);
        current_index_data += token_length;
    }

    dns_packet->data[current_index_data] = 0;
    current_index_data++;

    *(uint16_t*)&dns_packet->data[current_index_data] = htons(1);
    current_index_data += sizeof(uint16_t);
    *(uint16_t*)&dns_packet->data[current_index_data] = htons(1);

    generate_udp_packet(net_device, internal->dns_ip, htons(DNS_PORT), htons(DNS_PORT), dns_packet_size, dns_packet);

    int64_t timeout = 1000; // in ms
    while(!request.have_response && timeout > 0){
        timeout--;
        sleep_us(1000);
    }

    if(timeout == 0){
        *address = 0;
        *ttl = 0;
        *error = 6; // we add our error code : timeout
    }else{
        *address = request.address;
        *ttl = request.ttl;
        *error = request.error;    
    }

    vector_remove(dns_requests, request_index);

    return 0;
}