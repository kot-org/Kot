#ifndef _MODULE_NET_SOCKET_H
#define _MODULE_NET_SOCKET_H

#include <main.h>
#include <errno.h>
#include <protocols.h>
#include <global/socket.h>
#include <global/modules.h>

#define TIMEOUT_RECEIVE     5000

#define MIN_LISTENING_PORT 1024
#define MAX_LISTENING_PORT 65535

#define TYPE_TCP            1
#define TYPE_TCP_CHILD      2
#define TYPE_UDP            3

#define UDP_CACHE_SIZE      4096

typedef struct tcp_listener_buffer_t{
    size_t size;
    void* buffer;
    struct tcp_listener_buffer_t* next;
}tcp_listener_buffer_t;

typedef struct{
    net_device_t* net_device;
    uint32_t ip_address;
    uint16_t destination_port;
    uint16_t source_port;
    tcp_seq sequence_receive;
    tcp_seq sequence_send;
    tcp_seq sequence_waiting_for_ack;
    bool is_fin;
    bool is_reset;
    bool is_accept;
    bool is_waiting_for_accept;

    spinlock_t lock;
    
    /* all buffers */
    tcp_listener_buffer_t* last_allocated;
    tcp_listener_buffer_t* last_read;

    /* current buffer */
    void* buffer_read;
    tcp_seq start_offset;
    tcp_seq smallest_write_offset;
    size_t smallest_continuous_size;
    size_t buffer_read_size;
    size_t buffer_read_size_allocated;

    bool is_receiving_data;

    struct socket_internal_data_t* internal_data_parent;
}socket_tcp_listener_t;

typedef struct{
    uint64_t listen_index;
    int listen_allow_count;
    int listen_accept_count;

    bool is_connect;

    socket_tcp_listener_t* listeners;
}socket_tcp_data_t;

typedef struct{
    kernel_socket_t* parent;
    socket_tcp_listener_t* listener;
}socket_tcp_child_data_t;

typedef struct{
    uint64_t listen_index;
    spinlock_t lock;
    
    bool is_awaiting_read;
    void* buffer_read;
    vmm_space_t read_buffer_vmm_space;
    size_t buffer_read_size;
    size_t buffer_read_max_size;

    void* cache_buffer;
    size_t cache_size;
    size_t cache_size_used;
}socket_udp_data_t;


typedef struct socket_internal_data_t{
    struct sockaddr_in* address;
    socklen_t address_length;
    spinlock_t lock;

    uint8_t data_type;

    union{
        socket_tcp_data_t* tcp;
        socket_tcp_child_data_t* tcp_child;
        socket_udp_data_t* udp;
        void* unknow;
    }data;

    short events;  
}socket_internal_data_t;

int socket_connect_handler(kernel_socket_t* socket, const struct sockaddr* addr_ptr, socklen_t addr_length);
int socket_read_handler(void* buffer, size_t size, size_t* size_read, kernel_socket_t* socket);
int socket_write_handler(void* buffer, size_t size, size_t* size_write, kernel_socket_t* socket);int socket_seek_handler(off_t offset, int whence, off_t* new_offset, kernel_socket_t* socket);
int socket_ioctl_handler(uint32_t request, void* arg, int* ptr_result, kernel_socket_t* socket);
int socket_stat_handler(int flags, struct stat* statbuf, kernel_socket_t* socket);
int socket_close_handler(kernel_socket_t* socket);
int socket_listen_handler(kernel_socket_t* socket, int backlog);
int socket_bind_handler(kernel_socket_t* socket, const struct sockaddr* addr_ptr, socklen_t addr_length);
kernel_socket_t* socket_accept_handler(kernel_socket_t* socket, struct sockaddr* addr_ptr, socklen_t* addr_length, int* error);
kernel_socket_t* socket_handler(int family, int type, int protocol, int* error);
int socket_init(void);

#endif // _MODULE_NET_SOCKET_H