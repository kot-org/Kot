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

        send_arp_packet(net_device, __bswap_16(arp_header->ar_hrd), __bswap_16(arp_header->ar_pro), arp_header->ar_hln, arp_header->ar_pln, ARPOP_REPLY, net_device->mac_address, tip, sha, sip);
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
    memcpy(ar_sha, sha, hln);
    uint8_t* ar_tha = (uint8_t*)((uintptr_t)*header_buffer + (uintptr_t)sizeof(struct arphdr) + hln + pln);
    memcpy(ar_tha, tha, hln);

    uint8_t* ar_sip = (uint8_t*)((uintptr_t)*header_buffer + (uintptr_t)sizeof(struct arphdr) + hln);
    memcpy(ar_sip, sip, pln);

    uint8_t* ar_tip = (uint8_t*)((uintptr_t)*header_buffer + (uintptr_t)sizeof(struct arphdr) + hln + pln + hln);
    memcpy(ar_tip, tip, pln);
    
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