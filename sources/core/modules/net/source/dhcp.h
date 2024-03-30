#ifndef _MODULE_NET_DHCP_H
#define _MODULE_NET_DHCP_H

#include <dhcp_header.h>
#include <global/modules.h>

int init_dhcp(void);
int get_dhcp_info(net_device_t* net_device);

#endif // _MODULE_NET_DHCP_H    