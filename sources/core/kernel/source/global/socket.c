#include <global/socket.h>

socket_handler_t* socket_handlers = NULL;

int s_init(void){
    socket_handlers = (socket_handler_t*)calloc(AF_MAX, sizeof(socket_handler_t));

    return 0;
}

kernel_socket_t* s_socket(int family, int type, int protocol, int* error){
    assert(socket_handlers);

    if(family < 0 || family >= AF_MAX){
        *error = EAFNOSUPPORT;
        return NULL;
    }

    socket_handler_t handler = socket_handlers[family];

    if(handler == NULL){
        *error = EAFNOSUPPORT;
        return NULL;
    }

    return handler(family, type, protocol, error);
}

int s_add_family_handler(int family, socket_handler_t handler){
    assert(socket_handlers);

    if(family < 0 || family >= AF_MAX){
        return EAFNOSUPPORT;
    }

    socket_handlers[family] = handler;

    return 0;
}