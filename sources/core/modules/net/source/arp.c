#include <main.h>

vector_t* arp_table;

typedef struct{
    uint32_t ip;
    uint8_t mac[ETHER_ADDR_LEN];
}arp_entry_t;

static uint8_t* arp_table_find(uint32_t ip){
    for(uint64_t i = 0; i < arp_table->length; i++){
        arp_entry_t* entry = (arp_entry_t*)vector_get(arp_table, i);
        if(entry->ip == ip){
            return entry->mac; 
        }
    }
    return NULL;
}

static int arp_table_add(uint32_t ip, uint8_t* mac){
    if(arp_table_find(ip) == NULL){
        arp_entry_t* entry = (arp_entry_t*)malloc(sizeof(arp_entry_t));
        entry->ip = ip;
        memcpy(&entry->mac, mac, ETHER_ADDR_LEN);

        vector_push(arp_table, entry);

        return 0;
    }else{
        return -1;
    }
}

int init_arp(void){
    arp_table = vector_create();
    uint8_t broadcast_mac[8] = {0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff};
    arp_table_add(0xffffffff, broadcast_mac);
    return 0;
}

int process_arp_packet(net_device_t* net_device, size_t size, void* buffer){
    struct arphdr* arp_header = (struct arphdr*)buffer;
    
    #ifdef NET_DEBUG
    log_info("ARP header >\n\t- Hardware type : 0x%04x:0x%02x\n\t- Protocol type : 0x%04x:0x%02x\n\t- Operation Code : 0x%04x\n", 
    __bswap_16(arp_header->ar_hrd), arp_header->ar_hln,
    __bswap_16(arp_header->ar_pro), arp_header->ar_pln,
    __bswap_16(arp_header->ar_op));

    if(__bswap_16(arp_header->ar_hrd) == 1 && arp_header->ar_hln == ETHER_ADDR_LEN){
        uint8_t* sha = (uint8_t*)((uintptr_t)arp_header + (uintptr_t)sizeof(struct arphdr));
        uint8_t* tha = (uint8_t*)((uintptr_t)arp_header + (uintptr_t)sizeof(struct arphdr) + arp_header->ar_hln + arp_header->ar_pln);
        log_info("ARP MAC source: %02x:%02x:%02x:%02x:%02x:%02x\n", sha[0], sha[1], sha[2], sha[3], sha[4], sha[5]);
        log_info("ARP MAC destination: %02x:%02x:%02x:%02x:%02x:%02x\n", tha[0], tha[1], tha[2], tha[3], tha[4], tha[5]);
    }

    if(__bswap_16(arp_header->ar_pro) == ETHERTYPE_IP && arp_header->ar_pln == 4){
        uint8_t* sip = (uint8_t*)((uintptr_t)arp_header + (uintptr_t)sizeof(struct arphdr) + arp_header->ar_hln);
        uint8_t* tip = (uint8_t*)((uintptr_t)arp_header + (uintptr_t)sizeof(struct arphdr) + arp_header->ar_hln + arp_header->ar_pln + arp_header->ar_hln);
        log_info("ARP IP source: %d.%d.%d.%d\n", sip[0], sip[1], sip[2], sip[3]);
        log_info("ARP IP destination: %d.%d.%d.%d\n", tip[0], tip[1], tip[2], tip[3]);
    }
    #endif 
    
    if(__bswap_16(arp_header->ar_pro) == ETHERTYPE_IP && arp_header->ar_pln == 4 && __bswap_16(arp_header->ar_hrd) == 1 && arp_header->ar_hln == ETHER_ADDR_LEN){
        // TODO : check if this is our ip
        uint8_t* sha = (uint8_t*)((uintptr_t)arp_header + (uintptr_t)sizeof(struct arphdr));
        
        uint8_t* sip = (uint8_t*)((uintptr_t)arp_header + (uintptr_t)sizeof(struct arphdr) + arp_header->ar_hln);
        uint8_t* tip = (uint8_t*)((uintptr_t)arp_header + (uintptr_t)sizeof(struct arphdr) + arp_header->ar_hln + arp_header->ar_pln + arp_header->ar_hln);

        arp_table_add(*((uint32_t*)sip), sha);

        if(__bswap_16(arp_header->ar_op) == ARPOP_REQUEST){
            send_arp_packet(net_device, __bswap_16(arp_header->ar_hrd), __bswap_16(arp_header->ar_pro), arp_header->ar_hln, arp_header->ar_pln, ARPOP_REPLY, net_device->mac_address, tip, sha, sip);
        }
    }else{
        return -1;
    }
}

int generate_arp_header(net_device_t* net_device, uint16_t hrd, uint16_t pro, uint8_t hln, uint8_t pln, uint8_t op, uint8_t* sha, uint8_t* sip, uint8_t* tha, uint8_t* tip, size_t* header_size, struct arphdr** header_buffer){
    assert(header_buffer);
    *header_size = sizeof(struct arphdr) + (hln + pln) * 2;
    *header_buffer = malloc(*header_size);

    (*header_buffer)->ar_hrd = __bswap_16(hrd);
    (*header_buffer)->ar_pro = __bswap_16(pro);
    (*header_buffer)->ar_hln = hln;
    (*header_buffer)->ar_pln = pln;
    (*header_buffer)->ar_op = __bswap_16(op);

    uint8_t* ar_sha = (uint8_t*)((uintptr_t)*header_buffer + (uintptr_t)sizeof(struct arphdr));
    if(sha){
        memcpy(ar_sha, sha, hln);
    }else{
        memset(ar_sha, 0, hln);
    }

    uint8_t* ar_tha = (uint8_t*)((uintptr_t)*header_buffer + (uintptr_t)sizeof(struct arphdr) + hln + pln);
    if(tha){
        memcpy(ar_tha, tha, hln);
    }else{
        memset(ar_tha, 0, hln);
    }

    uint8_t* ar_sip = (uint8_t*)((uintptr_t)*header_buffer + (uintptr_t)sizeof(struct arphdr) + hln);
    if(sip){
        memcpy(ar_sip, sip, pln);
    }else{
        memset(ar_sip, 0, pln);
    }

    uint8_t* ar_tip = (uint8_t*)((uintptr_t)*header_buffer + (uintptr_t)sizeof(struct arphdr) + hln + pln + hln);
    if(tip){
        memcpy(ar_tip, tip, pln);
    }else{
        memset(ar_tip, 0, pln);
    }
    
    return 0;
}

int send_arp_packet(net_device_t* net_device, uint16_t hrd, uint16_t pro, uint8_t hln, uint8_t pln, uint8_t op, uint8_t* sha, uint8_t* sip, uint8_t* tha, uint8_t* tip){
    size_t header_size; 
    struct arphdr* header_buffer;

    int error = generate_arp_header(net_device, hrd, pro, hln, pln, op, sha, sip, tha, tip, &header_size, &header_buffer);

    if(error){
        return error;
    }

    error = send_ethernet_packet(net_device, tha, ETHERTYPE_ARP, header_size, header_buffer);

    free(header_buffer);

    return error;
}

uint8_t* arp_get_mac_address(net_device_t* net_device, uint32_t ip){
    uint8_t* mac = arp_table_find(ip);
    if(mac){
        return mac;
    }else{
        net_device_internal_t* internal = (net_device_internal_t*)net_device->external_data;
        send_arp_packet(net_device, 1, ETHERTYPE_IP, ETHER_ADDR_LEN, 4, ARPOP_REQUEST, net_device->mac_address, (uint8_t*)&internal->nic_ip, NULL, (uint8_t*)&ip);
        
        int timeout = 10;
        while(!mac && timeout--){
            sleep_us(1000);
            mac = arp_table_find(ip);
        }
        
        return mac;
    }
}