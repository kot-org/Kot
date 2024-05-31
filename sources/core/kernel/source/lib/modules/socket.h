#ifndef LIB_MODULES_SOCKET_H
#define LIB_MODULES_SOCKET_H 1

struct kernel_socket_t;

#include <stddef.h>
#include <stdint.h>
#include <dirent.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/socket.h>

typedef struct kernel_socket_t{
    int family;
    int type; 
    int protocol;
    void* internal_data;

    int (*read)(void*, size_t, size_t*, struct kernel_socket_t*);
    int (*write)(void*, size_t, size_t*, struct kernel_socket_t*);
    int (*seek)(off_t, int, off_t*, struct kernel_socket_t*);
    int (*ioctl)(uint32_t, void*, int*, struct kernel_socket_t*);
    int (*stat)(int, struct stat*, struct kernel_socket_t*);
    int (*close)(struct kernel_socket_t*);
    int (*listen)(struct kernel_socket_t*, int);
    int (*bind)(struct kernel_socket_t*, const struct sockaddr*, socklen_t);
    int (*connect)(struct kernel_socket_t*, const struct sockaddr*, socklen_t);
    int (*socket_send)(struct kernel_socket_t*, struct msghdr*, int, size_t*);
    int (*socket_recv)(struct kernel_socket_t*, struct msghdr*, int, size_t*);
    int (*socket_get_event)(struct kernel_socket_t*, short, short*);
    struct kernel_socket_t* (*accept)(struct kernel_socket_t*, struct sockaddr*, socklen_t*, int*);
} kernel_socket_t;

typedef kernel_socket_t* (*socket_handler_t)(int family, int type, int protocol, int* error);
typedef int (*socket_pair_handler_t)(int family, int type, int protocol, kernel_socket_t** first_socket, kernel_socket_t** second_socket);


#endif // LIB_MODULES_SOCKET_H