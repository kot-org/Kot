#include <main.h>

static size_t fill_dhcp_option(uint8_t* packet, uint8_t code, uint8_t* data, uint8_t length){
    packet[0] = code;
    packet[1] = length;
    memcpy(&packet[2], data, length);

    return length + (sizeof(uint8_t) * 2);
}

static void fill_dhcp_output(net_device_t* net_device, struct dhcp_packet* dhcp){
    dhcp->op = BOOTREQUEST;
    dhcp->htype = HTYPE_ETHER;
    dhcp->hlen = ETHER_ADDR_LEN;
    memcpy(dhcp->chaddr, net_device->mac_address, ETHER_ADDR_LEN);
    memcpy(dhcp->mcookie, DHCP_OPTIONS_COOKIE, DHCP_OPTIONS_COOKIE_SIZE);
}

static size_t fill_dhcp_discovery_options(struct dhcp_packet* dhcp){
    size_t length = 0;
    
    uint8_t option = DHCPDISCOVER;
    length += fill_dhcp_option(&dhcp->options[length], DHO_DHCP_MESSAGE_TYPE, &option, sizeof(option));
    
    uint8_t parameter_request_list[] = {DHO_SUBNET_MASK, DHO_DOMAIN_NAME_SERVERS, DHO_ROUTERS};
    length += fill_dhcp_option(&dhcp->options[length], DHO_DHCP_PARAMETER_REQUEST_LIST, (uint8_t*)&parameter_request_list, sizeof(parameter_request_list));
    
    option = 0;
    length += fill_dhcp_option(&dhcp->options[length], DHO_END, &option, sizeof(option));

    return length;
}

void receive_dhcp_info(void* external_data, net_device_t* net_device, struct udphdr* udp_header, size_t size){
    struct dhcp_packet* dhcp = (struct dhcp_packet*)((uintptr_t)udp_header + (uintptr_t)sizeof(struct udphdr));

    if(dhcp->op == BOOTREPLY && dhcp->htype == HTYPE_ETHER && dhcp->hlen == ETHER_ADDR_LEN){
        if(!memcmp(dhcp->chaddr, net_device->mac_address, ETHER_ADDR_LEN)){
            if(!memcmp(dhcp->mcookie, DHCP_OPTIONS_COOKIE, DHCP_OPTIONS_COOKIE_SIZE)){
                size_t length = 0;

                uint32_t dns_ip = 0;
                uint32_t router_ip = 0;
                uint8_t message_type = 0;
                uint32_t subnet_mask_ip = 0;

                while(dhcp->options[length] != DHO_END && length < DHCP_OPTION_LEN){
                    switch (dhcp->options[length]){
                        case DHO_DHCP_MESSAGE_TYPE:
                            message_type = dhcp->options[length + 2];
                            break;
                        case DHO_DOMAIN_NAME_SERVERS:
                            dns_ip = *(uint32_t*)&dhcp->options[length + 2];
                            break;
                        case DHO_SUBNET_MASK:
                            subnet_mask_ip = *(uint32_t*)&dhcp->options[length + 2];
                            break;
                        case DHO_ROUTERS:
                            router_ip = *(uint32_t*)&dhcp->options[length + 2];
                            break;
                        
                        default:
                            break;
                    }
                    length += dhcp->options[length + 1] + (sizeof(uint8_t) * 2);
                }

                if(message_type == DHCPOFFER){
                    net_device_internal_t* internal = (net_device_internal_t*)net_device->external_data;
                    internal->nic_ip = *(uint32_t*)&dhcp->yiaddr;
                    internal->dns_ip = dns_ip;
                    internal->router_ip = router_ip;
                    internal->subnet_mask_ip = subnet_mask_ip;

                    #ifdef NET_DEBUG
                    log_info("DHCPOFFER >\n\tNIC IP : %d.%d.%d.%d\n\tDNS IP : %d.%d.%d.%d\n\tSubnet mask IP : %d.%d.%d.%d\n\tRouter IP : %d.%d.%d.%d\n", 
                        ((uint8_t*)&internal->nic_ip)[0], ((uint8_t*)&internal->nic_ip)[1],((uint8_t*)&internal->nic_ip)[2],((uint8_t*)&internal->nic_ip)[3],
                        ((uint8_t*)&internal->dns_ip)[0], ((uint8_t*)&internal->dns_ip)[1],((uint8_t*)&internal->dns_ip)[2],((uint8_t*)&internal->dns_ip)[3],
                        ((uint8_t*)&internal->subnet_mask_ip)[0], ((uint8_t*)&internal->subnet_mask_ip)[1],((uint8_t*)&internal->subnet_mask_ip)[2],((uint8_t*)&internal->subnet_mask_ip)[3],
                        ((uint8_t*)&internal->router_ip)[0], ((uint8_t*)&internal->router_ip)[1],((uint8_t*)&internal->router_ip)[2],((uint8_t*)&internal->router_ip)[3]
                    );
                    #endif
                }
            }
        }
    }
}

int init_dhcp(void){
    udp_listen_port(DHCP_UDP_PORT_SOURCE, &receive_dhcp_info, NULL);
    return 0;
}

int get_dhcp_info(net_device_t* net_device){
    struct dhcp_packet dhcp = {};
    fill_dhcp_output(net_device, &dhcp);
    fill_dhcp_discovery_options(&dhcp);
    generate_udp_packet(net_device, 0xffffffff, __bswap_16(DHCP_UDP_PORT_DESTINATION), __bswap_16(DHCP_UDP_PORT_SOURCE), sizeof(struct dhcp_packet), &dhcp);
    return 0;
}