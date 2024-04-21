#ifndef _GLOBAL_SOCKET_H
#define _GLOBAL_SOCKET_H 1

#include <errno.h>
#include <stdint.h>
#include <stddef.h>
#include <sys/socket.h>
#include <lib/assert.h>
#include <global/heap.h>
#include <lib/modules/socket.h>

int s_init(void);
kernel_socket_t* s_socket(int family, int type, int protocol, int* error);
int s_add_family_handler(int family, socket_handler_t handler);


#endif // _GLOBAL_SOCKET_H