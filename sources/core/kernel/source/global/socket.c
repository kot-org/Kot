#include <global/socket.h>

typedef struct{
    socket_handler_t socket_handler;
    socket_pair_handler_t socket_pair_handler;
}socket_handler_data_t;

socket_handler_data_t* socket_handlers = NULL;

int s_init(void){
    socket_handlers = (socket_handler_data_t*)calloc(AF_MAX, sizeof(socket_handler_data_t));

    return 0;
}

kernel_socket_t* s_socket(int family, int type, int protocol, int* error){
    assert(socket_handlers);

    if(family < 0 || family >= AF_MAX){
        *error = EAFNOSUPPORT;
        return NULL;
    }

    socket_handler_t handler = socket_handlers[family].socket_handler;

    if(handler == NULL){
        *error = EAFNOSUPPORT;
        return NULL;
    }

    return handler(family, type, protocol, error);
}

int s_socket_pair(int family, int type, int protocol, kernel_socket_t** first_socket, kernel_socket_t** second_socket){
    assert(socket_handlers);

    if(family < 0 || family >= AF_MAX){
        return EAFNOSUPPORT;
    }

    socket_pair_handler_t handler = socket_handlers[family].socket_pair_handler;

    if(handler == NULL){
        return EAFNOSUPPORT;
    }

    return handler(family, type, protocol, first_socket, second_socket);
}

int s_add_family_handler(int family, socket_handler_t socket_handler, socket_pair_handler_t socket_pair_handler){
    assert(socket_handlers);

    if(family < 0 || family >= AF_MAX){
        return EAFNOSUPPORT;
    }

    socket_handlers[family].socket_handler = socket_handler;
    socket_handlers[family].socket_pair_handler = socket_pair_handler;

    return 0;
}