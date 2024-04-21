#include <main.h>

static int get_tcp_listener(int listen_accept_count, socket_tcp_listener_t* listeners, uint32_t ip_address, uint16_t port){
    for(int i = 0; i < listen_accept_count; i++){
        if(listeners[i].ip_address == ip_address && listeners[i].port == port){
            return i;
        }
    }
    return -1;
}

void tcp_socket_handler(void* external_data, net_device_t* net_device, struct tcphdr* tcp_header, size_t size, uint32_t saddr){
    kernel_socket_t* socket = (kernel_socket_t*)external_data;
    socket_internal_data_t* internal_data = (socket_internal_data_t*)socket->internal_data;
    
    if(internal_data->data.tcp->listeners != NULL){
        int index = get_tcp_listener(internal_data->data.tcp->listen_accept_count, internal_data->data.tcp->listeners, saddr, tcp_header->th_sport);
        
        if(index == -1){
            if(tcp_header->th_flags & TH_SYN){
                if(internal_data->data.tcp->listen_accept_count < internal_data->data.tcp->listen_allow_count){
                    for(int i = 0; i < internal_data->data.tcp->listen_allow_count; i++){
                        if(!internal_data->data.tcp->listeners[i].is_waiting_for_accept){
                            internal_data->data.tcp->listeners[i].net_device = net_device;
                            internal_data->data.tcp->listeners[i].ip_address = saddr;
                            internal_data->data.tcp->listeners[i].port = tcp_header->th_sport;
                            internal_data->data.tcp->listeners[i].sequence_receive = ntohl(tcp_header->th_seq);
                            internal_data->data.tcp->listeners[i].is_waiting_for_accept = true;
                            break;
                        }
                    }
                    internal_data->data.tcp->listen_accept_count++;
                }
            }
        }else{
            size_t data_offset = tcp_header->th_off * sizeof(uint32_t);
            size_t size_data = size - data_offset;

            if(internal_data->data.tcp->listeners[index].sequence_waiting_for_ack && tcp_header->th_flags & TH_ACK){
                internal_data->data.tcp->listeners[index].sequence_waiting_for_ack = 0;
            }

            if(tcp_header->th_flags & TH_RST){
                internal_data->data.tcp->listeners[index].is_reset = true;
            }

            if(tcp_header->th_flags & TH_SYN){
                internal_data->data.tcp->listeners[index].is_accept = true;
                internal_data->data.tcp->listeners[index].sequence_receive = ntohl(tcp_header->th_seq) + 1;
                internal_data->data.tcp->listeners[index].sequence_send++;

                generate_tcp_packet(
                    internal_data->data.tcp->listeners[index].net_device, 
                    internal_data->data.tcp->listeners[index].ip_address, 
                    internal_data->data.tcp->listeners[index].port, 
                    internal_data->address->sin_port, 
                    htonl(internal_data->data.tcp->listeners[index].sequence_send), 
                    htonl(internal_data->data.tcp->listeners[index].sequence_receive),
                    sizeof(struct tcphdr) / sizeof(uint32_t), 
                    TH_ACK, 
                    65535, 
                    0, 
                    0, 
                    NULL
                );
            }

            if(tcp_header->th_flags & TH_FIN){
                internal_data->data.tcp->listeners[index].is_fin = true;

                internal_data->data.tcp->listeners[index].sequence_receive++;
                /* Acknoledge fin */
                generate_tcp_packet(
                    internal_data->data.tcp->listeners[index].net_device, 
                    internal_data->data.tcp->listeners[index].ip_address, 
                    internal_data->data.tcp->listeners[index].port, 
                    internal_data->address->sin_port, 
                    htonl(internal_data->data.tcp->listeners[index].sequence_send), 
                    htonl(internal_data->data.tcp->listeners[index].sequence_receive),
                    sizeof(struct tcphdr) / sizeof(uint32_t), 
                    TH_ACK, 
                    65535, 
                    0, 
                    0, 
                    NULL
                );
            }

            if(size_data){
                if(internal_data->data.tcp->listeners[index].is_awaiting_read){
                    void* data = (void*)((uintptr_t)tcp_header + (uintptr_t)data_offset);
                    size_t size_to_copy = MIN(size_data, internal_data->data.tcp->listeners[index].buffer_read_max_size);

                    vmm_space_swap(internal_data->data.tcp->listeners[index].read_buffer_vmm_space);
                    memcpy(internal_data->data.tcp->listeners[index].buffer_read, data, size_to_copy);

                    internal_data->data.tcp->listeners[index].buffer_read_size = size_to_copy;

                    internal_data->data.tcp->listeners[index].is_awaiting_read = false;
                }

                internal_data->data.tcp->listeners[index].sequence_receive += size_data;

                /* Acknoledge data */
                generate_tcp_packet(
                    internal_data->data.tcp->listeners[index].net_device, 
                    internal_data->data.tcp->listeners[index].ip_address, 
                    internal_data->data.tcp->listeners[index].port, 
                    internal_data->address->sin_port, 
                    htonl(internal_data->data.tcp->listeners[index].sequence_send), 
                    htonl(internal_data->data.tcp->listeners[index].sequence_receive),
                    sizeof(struct tcphdr) / sizeof(uint32_t), 
                    TH_ACK, 
                    65535, 
                    0, 
                    0, 
                    NULL
                );
            }
        }
    }
}

int socket_read_handler(void* buffer, size_t size, size_t* size_read, kernel_socket_t* socket){
    if(vmm_check_memory(vmm_get_current_space(), (memory_range_t){buffer, size})){
        return EINVAL;
    }

    socket_internal_data_t* internal_data = (socket_internal_data_t*)socket->internal_data;

    switch(internal_data->data_type){
        case TYPE_TCP_CHILD:{
            spinlock_acquire(&internal_data->data.tcp_child->listener->lock);
            internal_data->data.tcp_child->listener->buffer_read = buffer;
            internal_data->data.tcp_child->listener->read_buffer_vmm_space = vmm_get_current_space();
            internal_data->data.tcp_child->listener->buffer_read_size = 0;
            internal_data->data.tcp_child->listener->buffer_read_max_size = size;
            internal_data->data.tcp_child->listener->is_awaiting_read = true;

            while(internal_data->data.tcp_child->listener->is_awaiting_read){
                kernel_sleep_us(1000);
            }

            memcpy(buffer, internal_data->data.tcp_child->listener->buffer_read, internal_data->data.tcp_child->listener->buffer_read_size);
            *size_read = internal_data->data.tcp_child->listener->buffer_read_size;

            spinlock_release(&internal_data->data.tcp_child->listener->lock);
            return 0;
        }   
        case TYPE_TCP:{
            if(internal_data->data.tcp->is_connect){
                spinlock_acquire(&internal_data->data.tcp->listeners[0].lock);
                internal_data->data.tcp->listeners[0].buffer_read = buffer;
                internal_data->data.tcp->listeners[0].read_buffer_vmm_space = vmm_get_current_space();
                internal_data->data.tcp->listeners[0].buffer_read_size = 0;
                internal_data->data.tcp->listeners[0].buffer_read_max_size = size;
                internal_data->data.tcp->listeners[0].is_awaiting_read = true;

                while(internal_data->data.tcp->listeners[0].is_awaiting_read){
                    kernel_sleep_us(1000);
                }

                memcpy(buffer, internal_data->data.tcp->listeners[0].buffer_read, internal_data->data.tcp->listeners[0].buffer_read_size);
                *size_read = internal_data->data.tcp->listeners[0].buffer_read_size;

                spinlock_release(&internal_data->data.tcp->listeners[0].lock);
            }
            return 0;
        }   
        default:
            return EINVAL;
    }   
}

int socket_write_handler(void* buffer, size_t size, size_t* size_write, kernel_socket_t* socket){
    if(vmm_check_memory(vmm_get_current_space(), (memory_range_t){buffer, size})){
        return EINVAL;
    }

    socket_internal_data_t* internal_data = (socket_internal_data_t*)socket->internal_data;

    switch(internal_data->data_type){
        case TYPE_TCP_CHILD:{
            spinlock_acquire(&internal_data->data.tcp_child->listener->lock);

            internal_data->data.tcp_child->listener->sequence_waiting_for_ack = internal_data->data.tcp_child->listener->sequence_send;
            
            uint64_t time_count = 0;

            while(time_count < TCP_TIME_OUT){
                generate_tcp_packet(
                    internal_data->data.tcp_child->listener->net_device, 
                    internal_data->data.tcp_child->listener->ip_address, 
                    internal_data->data.tcp_child->listener->port, 
                    internal_data->address->sin_port, 
                    htonl(internal_data->data.tcp_child->listener->sequence_send), 
                    htonl(internal_data->data.tcp_child->listener->sequence_receive),
                    sizeof(struct tcphdr) / sizeof(uint32_t), 
                    TH_ACK | TH_PUSH, 
                    65535, 
                    0, 
                    size, 
                    buffer
                );

                while(internal_data->data.tcp_child->listener->sequence_waiting_for_ack){
                    time_count++;
                    kernel_sleep_us(1000);

                    if(!(time_count % TCP_TIME_RETRANSMISSION)){
                        break;
                    }
                }

                if(!internal_data->data.tcp_child->listener->sequence_waiting_for_ack){
                    internal_data->data.tcp_child->listener->sequence_send += size;
                    *size_write = size;

                    spinlock_release(&internal_data->data.tcp_child->listener->lock);
                    return 0;
                }
            }

            spinlock_release(&internal_data->data.tcp_child->listener->lock);

            return EPIPE;
        }    
        case TYPE_TCP:{
            if(internal_data->data.tcp->is_connect){   
                spinlock_acquire(&internal_data->data.tcp->listeners[0].lock);

                internal_data->data.tcp->listeners[0].sequence_waiting_for_ack = internal_data->data.tcp->listeners[0].sequence_send;
                
                uint64_t time_count = 0;

                while(time_count < TCP_TIME_OUT){
                    generate_tcp_packet(
                        internal_data->data.tcp->listeners[0].net_device, 
                        internal_data->data.tcp->listeners[0].ip_address, 
                        internal_data->data.tcp->listeners[0].port, 
                        internal_data->address->sin_port, 
                        htonl(internal_data->data.tcp->listeners[0].sequence_send), 
                        htonl(internal_data->data.tcp->listeners[0].sequence_receive),
                        sizeof(struct tcphdr) / sizeof(uint32_t), 
                        TH_ACK | TH_PUSH, 
                        65535, 
                        0, 
                        size, 
                        buffer
                    );

                    while(internal_data->data.tcp->listeners[0].sequence_waiting_for_ack){
                        time_count++;
                        kernel_sleep_us(1000);

                        if(!(time_count % TCP_TIME_RETRANSMISSION)){
                            break;
                        }
                    }

                    if(!internal_data->data.tcp->listeners[0].sequence_waiting_for_ack){
                        internal_data->data.tcp->listeners[0].sequence_send += size;
                        *size_write = size;

                        spinlock_release(&internal_data->data.tcp->listeners[0].lock);
                        return 0;
                    }
                }

                spinlock_release(&internal_data->data.tcp->listeners[0].lock);

                return EPIPE;
            }
        }   
        default:
            return EINVAL;
    }

    return 0;
}
int socket_seek_handler(off_t offset, int whence, off_t* new_offset, kernel_socket_t* socket){
    // TODO

    return 0;
}

int socket_ioctl_handler(uint32_t request, void* arg, int* ptr_result, kernel_socket_t* socket){
    // TODO

    return 0;
}

int socket_stat_handler(int flags, struct stat* statbuf, kernel_socket_t* socket){
    // TODO

    return 0;
}

int socket_close_handler(kernel_socket_t* socket){
    socket_internal_data_t* internal_data = (socket_internal_data_t*)socket->internal_data;

    switch(internal_data->data_type){
        case TYPE_TCP_CHILD:{
            spinlock_acquire(&internal_data->data.tcp_child->listener->lock);

            internal_data->data.tcp_child->listener->sequence_waiting_for_ack = internal_data->data.tcp_child->listener->sequence_send;

            uint64_t time_count = 0;

            while(time_count < TCP_TIME_OUT){
                generate_tcp_packet(
                    internal_data->data.tcp_child->listener->net_device, 
                    internal_data->data.tcp_child->listener->ip_address, 
                    internal_data->data.tcp_child->listener->port, 
                    internal_data->address->sin_port, 
                    htonl(internal_data->data.tcp_child->listener->sequence_send), 
                    htonl(internal_data->data.tcp_child->listener->sequence_receive),
                    sizeof(struct tcphdr) / sizeof(uint32_t), 
                    TH_FIN | TH_ACK, 
                    65535, 
                    0, 
                    0, 
                    NULL
                );

                while(internal_data->data.tcp_child->listener->sequence_waiting_for_ack || !internal_data->data.tcp_child->listener->is_fin){
                    time_count++;
                    kernel_sleep_us(1000);

                    if(!(time_count % TCP_TIME_RETRANSMISSION)){
                        break;
                    }
                }

                if(!internal_data->data.tcp_child->listener->sequence_waiting_for_ack && internal_data->data.tcp_child->listener->is_fin){
                    internal_data->data.tcp_child->listener->sequence_send++;

                    spinlock_release(&internal_data->data.tcp_child->listener->lock);
                    
                    internal_data->data.tcp_child->listener->is_waiting_for_accept = false;
                    internal_data->data.tcp_child->listener->is_accept = false;

                    socket_internal_data_t* internal_data_parent = (socket_internal_data_t*)internal_data->data.tcp_child->parent->internal_data;
                    internal_data_parent->data.tcp->listen_accept_count--;

                    return 0;
                }
            }

            spinlock_release(&internal_data->data.tcp_child->listener->lock);

            return EIO;
        }          
        default:
            return EINVAL;
    }

    return 0;
}

int socket_listen_handler(kernel_socket_t* socket, int backlog){
    socket_internal_data_t* internal_data = (socket_internal_data_t*)socket->internal_data;

    if(internal_data->data.tcp->listeners == NULL){
        internal_data->data.tcp->listen_allow_count = backlog;
        internal_data->data.tcp->listeners = calloc(backlog, sizeof(socket_tcp_listener_t));

        for(int i = 0; i < backlog; i++){
            internal_data->data.tcp->listeners[i].lock = (spinlock_t)SPINLOCK_INIT;
        }
    }else{
        // TODO
    }

    return 0;
}

int socket_bind_handler(kernel_socket_t* socket, const struct sockaddr* addr_ptr, socklen_t addr_length){
    if(addr_length < sizeof(struct sockaddr_in)){
        return EINVAL;
    }

    if(vmm_check_memory(vmm_get_current_space(), (memory_range_t){(void*)addr_ptr, addr_length})){
        return EINVAL;
    }

    socket_internal_data_t* internal_data = (socket_internal_data_t*)socket->internal_data;

    spinlock_acquire(&internal_data->lock);

    struct sockaddr_in* address = (struct sockaddr_in*)malloc(addr_length);
    memcpy(address, addr_ptr, addr_length);

    internal_data->address_length = 0;
    if(internal_data != NULL){
        switch(socket->type){
            case SOCK_STREAM:{
                tcp_remove_listen_port(address->sin_port, internal_data->data.tcp->listen_index);
                free(internal_data->address);
            }
        }
    }

    internal_data->address = address;
    internal_data->address_length = addr_length;


    switch(socket->type){
        case SOCK_STREAM:
            internal_data->data.tcp->listen_index = tcp_listen_port(address->sin_port, &tcp_socket_handler, socket);
            break;
        case SOCK_DGRAM:
            // TODO
            break;
        default:
            break;
    }

    spinlock_release(&internal_data->lock);

    return 0;
}

int socket_connect_handler(kernel_socket_t* socket, const struct sockaddr* addr_ptr, socklen_t addr_length){
    if(addr_length < sizeof(struct sockaddr_in)){
        return EINVAL;
    }

    if(vmm_check_memory(vmm_get_current_space(), (memory_range_t){(void*)addr_ptr, addr_length})){
        return EINVAL;
    }

    socket_internal_data_t* internal_data = (socket_internal_data_t*)socket->internal_data;

    spinlock_acquire(&internal_data->lock);

    struct sockaddr_in* address = (struct sockaddr_in*)malloc(addr_length);
    memcpy(address, addr_ptr, addr_length);

    internal_data->address_length = 0;
    if(internal_data != NULL){
        switch(socket->type){
            case SOCK_STREAM:{
                tcp_remove_listen_port(address->sin_port, internal_data->data.tcp->listen_index);
                free(internal_data->address);
            }
        }
    }

    internal_data->address = address;
    internal_data->address_length = addr_length;


    switch(socket->type){
        case SOCK_STREAM:{
            internal_data->data.tcp->listen_index = tcp_listen_port(address->sin_port, &tcp_socket_handler, socket);

            internal_data->data.tcp->listen_allow_count = 0;
            internal_data->data.tcp->listen_accept_count = 1;
            internal_data->data.tcp->listeners = calloc(1, sizeof(socket_tcp_listener_t));

            internal_data->data.tcp->listeners[0].lock = (spinlock_t)SPINLOCK_INIT;
            internal_data->data.tcp->listeners[0].net_device = get_main_net_device();
            internal_data->data.tcp->listeners[0].ip_address = address->sin_addr.s_addr;
            internal_data->data.tcp->listeners[0].port = address->sin_port;
           
    

            /* Initialize connection */

            spinlock_acquire(&internal_data->data.tcp->listeners[0].lock);

            uint64_t time_count = 0;

            while(time_count < TCP_TIME_OUT){
                generate_tcp_packet(
                    internal_data->data.tcp->listeners[0].net_device, 
                    internal_data->data.tcp->listeners[0].ip_address, 
                    internal_data->data.tcp->listeners[0].port, 
                    internal_data->address->sin_port, 
                    htonl(internal_data->data.tcp->listeners[0].sequence_send), 
                    htonl(0),
                    sizeof(struct tcphdr) / sizeof(uint32_t), 
                    TH_SYN, 
                    65535, 
                    0, 
                    0, 
                    NULL
                );

                while(!internal_data->data.tcp->listeners[0].is_accept && !internal_data->data.tcp->listeners[0].is_reset){
                    time_count++;
                    kernel_sleep_us(1000);

                    if(!(time_count % TCP_TIME_RETRANSMISSION)){
                        break;
                    }
                }

                if(internal_data->data.tcp->listeners[0].is_accept){
                    internal_data->data.tcp->is_connect = true;
                    spinlock_release(&internal_data->data.tcp->listeners[0].lock);

                    return 0;
                }else if(internal_data->data.tcp->listeners[0].is_reset){
                    spinlock_release(&internal_data->data.tcp->listeners[0].lock);

                    return ECONNREFUSED;                    
                }
            }

            spinlock_release(&internal_data->data.tcp->listeners[0].lock);

            return ETIMEDOUT;
        }
        case SOCK_DGRAM:
            // TODO
            break;
        default:
            break;
    }

    spinlock_release(&internal_data->lock);

    return 0;
}

kernel_socket_t* socket_accept_handler(kernel_socket_t* socket, struct sockaddr* addr_ptr, socklen_t* addr_length, int* error){
    if(addr_ptr != NULL && addr_length != NULL){
        if(vmm_check_memory(vmm_get_current_space(), (memory_range_t){(void*)addr_length, sizeof(socklen_t)})){
            *error = EINVAL;
            return NULL;
        }

        if(vmm_check_memory(vmm_get_current_space(), (memory_range_t){(void*)addr_ptr, *addr_length})){
            *error = EINVAL;
            return NULL;
        }
    }


    kernel_socket_t* socket_child = malloc(sizeof(kernel_socket_t));

    socket_child->internal_data = calloc(1, sizeof(socket_internal_data_t));
    socket_child->family = socket->family;
    socket_child->type = socket->type;
    socket_child->protocol = socket->protocol;
    socket_child->bind = &socket_bind_handler;
    socket_child->connect = &socket_connect_handler;
    socket_child->listen = &socket_listen_handler;
    socket_child->accept = &socket_accept_handler;
    socket_child->read = &socket_read_handler;
    socket_child->write = &socket_write_handler;
    socket_child->seek = &socket_seek_handler;
    socket_child->ioctl = &socket_ioctl_handler;
    socket_child->stat = &socket_stat_handler;
    socket_child->close = &socket_close_handler;

    socket_internal_data_t* internal_data = (socket_internal_data_t*)socket->internal_data;
    socket_internal_data_t* child_internal_data = (socket_internal_data_t*)socket_child->internal_data;
    child_internal_data->lock = (spinlock_t)SPINLOCK_INIT;

    child_internal_data->address = internal_data->address;
    child_internal_data->address_length = internal_data->address_length;

    switch(socket->type){
        case SOCK_STREAM:
            int index = 0;

            while(internal_data->data.tcp->listeners[index].is_accept || !internal_data->data.tcp->listeners[index].is_waiting_for_accept){
                index++;
                if(index >= internal_data->data.tcp->listen_accept_count){
                    index = 0;
                } 
            }

            internal_data->data.tcp->listeners[index].is_fin = false;

            internal_data->data.tcp->listeners[index].is_accept = true;

            internal_data->data.tcp->listeners[index].sequence_send = 0; // to do make it random

            internal_data->data.tcp->listeners[index].sequence_receive++;
            
            generate_tcp_packet(
                internal_data->data.tcp->listeners[index].net_device, 
                internal_data->data.tcp->listeners[index].ip_address, 
                internal_data->data.tcp->listeners[index].port, 
                internal_data->address->sin_port, 
                htonl(internal_data->data.tcp->listeners[index].sequence_send), 
                htonl(internal_data->data.tcp->listeners[index].sequence_receive),
                sizeof(struct tcphdr) / sizeof(uint32_t), 
                TH_SYN | TH_ACK, 
                65535, 
                0, 
                0, 
                NULL
            );

            internal_data->data.tcp->listeners[index].sequence_send++;

            child_internal_data->data_type = TYPE_TCP_CHILD;
            child_internal_data->data.tcp_child = calloc(1, sizeof(socket_tcp_child_data_t));
            child_internal_data->data.tcp_child->parent = socket;
            child_internal_data->data.tcp_child->listener = &internal_data->data.tcp->listeners[index];
            break;
        case SOCK_DGRAM:
            // TODO
            break;
        default:
            break;
    }

    *error = 0;
    return socket_child;
}

kernel_socket_t* socket_handler(int family, int type, int protocol, int* error){
    if(protocol == 0){
        kernel_socket_t* socket = malloc(sizeof(kernel_socket_t));

        socket->internal_data = calloc(1, sizeof(socket_internal_data_t));
        socket->family = family;
        socket->type = type;
        socket->protocol = protocol;
        socket->bind = &socket_bind_handler;
        socket->connect = &socket_connect_handler;
        socket->listen = &socket_listen_handler;
        socket->accept = &socket_accept_handler;
        socket->read = &socket_read_handler;
        socket->write = &socket_write_handler;
        socket->seek = &socket_seek_handler;
        socket->ioctl = &socket_ioctl_handler;
        socket->stat = &socket_stat_handler;
        socket->close = &socket_close_handler;

        socket_internal_data_t* internal_data = (socket_internal_data_t*)socket->internal_data;
        internal_data->lock = (spinlock_t)SPINLOCK_INIT;

        switch(socket->type){
            case SOCK_STREAM:
                internal_data->data_type = TYPE_TCP;
                internal_data->data.tcp = calloc(1, sizeof(socket_tcp_data_t));
                break;
            case SOCK_DGRAM:
                // TODO
                break;
            default:
                break;
        }

        *error = 0;
        return socket;
    }else{
        *error = EPROTONOSUPPORT;
        return NULL;
    }
}

int socket_init(void){
    return s_add_family_handler(AF_INET, &socket_handler);
}