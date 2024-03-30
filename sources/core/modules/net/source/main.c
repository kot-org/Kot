#include <main.h>

#define MODULE_NAME "net"

#include <ip.c>
#include <arp.c>
#include <tcp.c>
#include <udp.c>
#include <dhcp.c>
#include <ethernet.c>
#include <interface.c>

int init(int argc, char* args[]){
    log_printf("[module/"MODULE_NAME"] loading start\n");

    assert(!init_ip());
    assert(!init_arp());
    assert(!init_udp());
    assert(!init_tcp());
    assert(!init_dhcp());
    assert(!init_ethernet());
    assert(!interface_init());

    log_printf("[module/"MODULE_NAME"] loading end\n");
    return 0;
}

int fini(void){
    return 0;
}

module_metadata_t module_metadata = {
    &init,
    &fini,
    MODULE_TYPE_NET,
    MODULE_NAME
};
