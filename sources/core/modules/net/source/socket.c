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
                            internal_data->data.tcp->listeners[i].last_allocated = malloc(sizeof(tcp_listener_buffer_t));
                            internal_data->data.tcp->listeners[i].last_allocated->size = 0;
                            internal_data->data.tcp->listeners[i].last_read = malloc(sizeof(tcp_listener_buffer_t));
                            internal_data->data.tcp->listeners[i].last_read->buffer = NULL;
                            internal_data->data.tcp->listeners[i].last_read->next = internal_data->data.tcp->listeners[i].last_allocated;
                            internal_data->data.tcp->listeners[i].internal_data_parent = internal_data;
                            break;
                        }
                    }
                    internal_data->data.tcp->listen_accept_count++;
                }
            }
        }else{
            socket_tcp_listener_t* listener = &internal_data->data.tcp->listeners[index];

            size_t data_offset = tcp_header->th_off * sizeof(uint32_t);
            size_t size_data = size - data_offset;

            if(listener->sequence_waiting_for_ack && tcp_header->th_flags & TH_ACK){
                listener->sequence_waiting_for_ack = 0;
            }

            if(tcp_header->th_flags & TH_RST){
                listener->is_reset = true;
            }

            if(tcp_header->th_flags & TH_SYN){
                listener->is_accept = true;
                listener->sequence_receive = ntohl(tcp_header->th_seq) + 1;
                listener->sequence_send++;
            }
            
            if(size_data){
                void* data = (void*)((uintptr_t)tcp_header + (uintptr_t)data_offset);

                size_t awaiting_data_to_read = listener->buffer_read_size_allocated - listener->buffer_read_size;
                size_t next_sequence = ntohl(tcp_header->th_seq) + size_data;
                
                if(awaiting_data_to_read == 0){
                    listener->buffer_read_size_allocated = next_sequence - listener->sequence_receive;
                    listener->buffer_read = malloc(listener->buffer_read_size_allocated);
                    listener->buffer_read_size = 0;
                    listener->start_offset = listener->sequence_receive;

                }

                if(listener->buffer_read_size + size_data <= listener->buffer_read_size_allocated){
                    size_t offset = listener->sequence_receive - listener->start_offset;
                    void* data_dst = (void*)((uintptr_t)listener->buffer_read + (uintptr_t)offset);

                    memcpy(data_dst, data, size_data);
                    listener->buffer_read_size += size_data;
                    listener->sequence_receive = next_sequence;
                }

                if(listener->buffer_read_size_allocated == listener->buffer_read_size){
                    tcp_listener_buffer_t* next_data_buffer = malloc(sizeof(tcp_listener_buffer_t));
                    next_data_buffer->size = 0; /* invalid the next data for now */

                    listener->last_allocated->next = next_data_buffer;
                    listener->last_allocated->buffer = listener->buffer_read;
                    listener->last_allocated->size = listener->buffer_read_size_allocated;

                    listener->last_allocated = next_data_buffer; /* update the last allocated */
                }
            }


            if(tcp_header->th_flags & TH_FIN){
                listener->is_fin = true;

                listener->sequence_receive++;
            }

            /* Acknoledge */
            generate_tcp_packet(
                listener->net_device, 
                listener->ip_address, 
                listener->port, 
                internal_data->address->sin_port, 
                htonl(listener->sequence_send), 
                htonl(listener->sequence_receive),
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

void udp_socket_handler(void* external_data, net_device_t* net_device, struct udphdr* udp_header, size_t size, uint32_t saddr){
    kernel_socket_t* socket = (kernel_socket_t*)external_data;
    socket_internal_data_t* internal_data = (socket_internal_data_t*)socket->internal_data;

    size_t data_offset = sizeof(struct udphdr);
    size_t size_data = size - data_offset;

    if(size_data){
        if(internal_data->data.udp->is_awaiting_read){
            void* data = (void*)((uintptr_t)udp_header + (uintptr_t)data_offset);
            size_t size_to_copy = MIN(size_data, internal_data->data.udp->buffer_read_max_size);

            vmm_space_swap(internal_data->data.udp->read_buffer_vmm_space);
            memcpy(internal_data->data.udp->buffer_read, data, size_to_copy);

            internal_data->data.udp->buffer_read_size = size_to_copy;

            internal_data->data.udp->is_awaiting_read = false;
        }else if(!internal_data->data.udp->cache_size_used){
            void* data = (void*)((uintptr_t)udp_header + (uintptr_t)data_offset);
            size_t size_to_copy = MIN(size_data, internal_data->data.udp->cache_size);

            memcpy(internal_data->data.udp->cache_buffer, data, size_to_copy);

            internal_data->data.udp->cache_size_used = size_data;
        }
    }
}

int socket_general_receive_handler(void* buffer, size_t size, size_t* size_receive, kernel_socket_t* socket, struct sockaddr* addr_ptr, socklen_t addr_length, int flags){
    if(vmm_check_memory(vmm_get_current_space(), (memory_range_t){buffer, size})){
        return EINVAL;
    }

    if(addr_ptr != NULL){
        if(vmm_check_memory(vmm_get_current_space(), (memory_range_t){addr_ptr, addr_length})){
            return EINVAL;
        }
    }

    socket_internal_data_t* internal_data = (socket_internal_data_t*)socket->internal_data;

    switch(internal_data->data_type){
        case TYPE_TCP_CHILD:{
            socket_tcp_listener_t* listener = internal_data->data.tcp_child->listener;
            spinlock_acquire(&listener->lock);

            if(listener->last_read != NULL){
                size_t size_to_read = size;
                uintptr_t offset = 0;
                while((listener->last_read->next->size > 0) && (size_to_read > 0)){
                    tcp_listener_buffer_t* data = listener->last_read->next;
                    size_t size_reading = MIN(size_to_read, data->size); 
                    void* buffer_to_copy = (void*)(offset + (uintptr_t)buffer);
                    memcpy(buffer_to_copy, data->buffer, size_reading);

                    if(size_reading < data->size){
                        data->size -= size_reading;
                        void* new_buffer = malloc(data->size);
                        memcpy(new_buffer, data->buffer, data->size);
                        free(data->buffer);
                        data->buffer = new_buffer;
                    }else{
                        free(data->buffer);
                        /* DO NOT free the current data */
                        free(listener->last_read);
                        listener->last_read = data;
                    }

                    size_to_read -= size_reading;
                    offset += size_reading;

                }

                *size_receive = size - size_to_read;
            }

            spinlock_release(&listener->lock);
            return 0;
        }   
        case TYPE_TCP:{
            if(internal_data->data.tcp->is_connect){
                socket_tcp_listener_t* listener = &internal_data->data.tcp->listeners[0];
                spinlock_acquire(&listener->lock);

                if(listener->last_read != NULL){
                    size_t size_to_read = size;
                    uintptr_t offset = 0;
                    while((listener->last_read->next->size > 0) && (size_to_read > 0)){
                        tcp_listener_buffer_t* data = listener->last_read->next;
                        size_t size_reading = MIN(size_to_read, data->size); 
                        void* buffer_to_copy = (void*)(offset + (uintptr_t)buffer);
                        memcpy(buffer_to_copy, data->buffer, size_reading);

                        if(size_reading < data->size){
                            data->size -= size_reading;
                            void* new_buffer = malloc(data->size);
                            memcpy(new_buffer, data->buffer, data->size);
                            free(data->buffer);
                            data->buffer = new_buffer;
                        }else{
                            free(data->buffer);
                            /* DO NOT free the current data */
                            free(listener->last_read);
                            listener->last_read = data;
                        }

                        size_to_read -= size_reading;
                        offset += size_reading;

                    }

                    *size_receive = size - size_to_read;
                }

                spinlock_release(&listener->lock);
                
                return 0;
            }else{
                *size_receive = 0;
                return EINVAL;
            }
        }  
        case TYPE_UDP:{
            spinlock_acquire(&internal_data->data.udp->lock);
            if(internal_data->data.udp->cache_size_used){
                size_t size_to_copy = MIN(size, internal_data->data.udp->cache_size_used);
                memcpy(buffer, internal_data->data.udp->cache_buffer, size_to_copy);
                *size_receive = size_to_copy;
            }else{
                internal_data->data.udp->buffer_read = buffer;
                internal_data->data.udp->read_buffer_vmm_space = vmm_get_current_space();
                internal_data->data.udp->buffer_read_size = 0;
                internal_data->data.udp->buffer_read_max_size = size;
                internal_data->data.udp->is_awaiting_read = true;

                while(internal_data->data.udp->is_awaiting_read){
                    kernel_sleep_us(1000);
                }

                *size_receive = internal_data->data.udp->buffer_read_size;

            }

            if(addr_ptr != NULL && addr_length >= internal_data->address_length){
                memcpy(addr_ptr, internal_data->address, internal_data->address_length);
            }

            spinlock_release(&internal_data->data.udp->lock);
            return 0;
        } 
        default:
            *size_receive = 0;
            return EINVAL;
    }   
}

int socket_general_send_handler(void* buffer, size_t size, size_t* size_send, kernel_socket_t* socket, struct sockaddr* addr_ptr, socklen_t addr_length, int flags){
    if(vmm_check_memory(vmm_get_current_space(), (memory_range_t){buffer, size})){
        return EINVAL;
    }

    if(addr_ptr != NULL){
        if(vmm_check_memory(vmm_get_current_space(), (memory_range_t){addr_ptr, addr_length})){
            return EINVAL;
        }
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
                    *size_send = size;

                    spinlock_release(&internal_data->data.tcp_child->listener->lock);
                    return 0;
                }
            }

            spinlock_release(&internal_data->data.tcp_child->listener->lock);

            *size_send = 0;
            return EPIPE;
        }    
        case TYPE_TCP:{
            if(addr_ptr != NULL){
                socket_connect_handler(socket, addr_ptr, addr_length);
            }

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
                        *size_send = size;

                        spinlock_release(&internal_data->data.tcp->listeners[0].lock);
                        return 0;
                    }
                }

                spinlock_release(&internal_data->data.tcp->listeners[0].lock);

                *size_send = 0;
                return EPIPE;
            }
        }
        case TYPE_UDP:{
            uint32_t daddress;
            uint16_t dport;
            uint16_t sport;

            if(addr_ptr != NULL){
                daddress = ((struct sockaddr_in*)addr_ptr)->sin_addr.s_addr;
                dport = ((struct sockaddr_in*)addr_ptr)->sin_port;
                sport = dport;

                struct sockaddr_in* address = (struct sockaddr_in*)malloc(addr_length);
                memcpy(address, addr_ptr, addr_length);

                if(internal_data->address != NULL){
                    udp_remove_listen_port(internal_data->address->sin_port, internal_data->data.udp->listen_index);
                    free(internal_data->address);
                }

                internal_data->address = address;
                internal_data->address_length = addr_length;
                internal_data->data.udp->listen_index = udp_listen_port(address->sin_port, &udp_socket_handler, socket);
            }else if(internal_data->address != NULL){
                daddress = internal_data->address->sin_addr.s_addr;
                dport = internal_data->address->sin_port;
                sport = dport;
            }else{
                return EINVAL;
            }

            int error = generate_udp_packet(get_main_net_device(), daddress, dport, sport, size, buffer);

            if(!error){
                *size_send = size;
            }

            return error;
        }
        default:
            *size_send = 0;
            return EINVAL;
    }
}

int socket_read_handler(void* buffer, size_t size, size_t* size_read, kernel_socket_t* socket){
    return socket_general_receive_handler(buffer, size, size_read, socket, NULL, 0, 0);
}

int socket_write_handler(void* buffer, size_t size, size_t* size_write, kernel_socket_t* socket){
    return socket_general_send_handler(buffer, size, size_write, socket, NULL, 0, 0);
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
                        TH_FIN | TH_ACK, 
                        65535, 
                        0, 
                        0, 
                        NULL
                    );

                    while(internal_data->data.tcp->listeners[0].sequence_waiting_for_ack || !internal_data->data.tcp->listeners[0].is_fin){
                        time_count++;
                        kernel_sleep_us(1000);

                        if(!(time_count % TCP_TIME_RETRANSMISSION)){
                            break;
                        }
                    }

                    if(!internal_data->data.tcp->listeners[0].sequence_waiting_for_ack && internal_data->data.tcp->listeners[0].is_fin){
                        break;
                    }
                }

                spinlock_release(&internal_data->data.tcp->listeners[0].lock);   

                if(time_count >= TCP_TIME_OUT){
                    return EIO;
                }
            }
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
        switch(internal_data->data_type){
            case TYPE_TCP:{
                tcp_remove_listen_port(internal_data->address->sin_port, internal_data->data.tcp->listen_index);
                free(internal_data->address);
                break;
            }
            case TYPE_UDP:{
                udp_remove_listen_port(internal_data->address->sin_port, internal_data->data.udp->listen_index);
                free(internal_data->address);
                break;
            }
        }
    }

    internal_data->address = address;
    internal_data->address_length = addr_length;


    switch(internal_data->data_type){
        case TYPE_TCP:
            internal_data->data.tcp->listen_index = tcp_listen_port(address->sin_port, &tcp_socket_handler, socket);
            break;
        case TYPE_UDP:
            internal_data->data.udp->listen_index = udp_listen_port(address->sin_port, &udp_socket_handler, socket);
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
        switch(internal_data->data_type){
            case TYPE_TCP:{
                tcp_remove_listen_port(address->sin_port, internal_data->data.tcp->listen_index);
                free(internal_data->address);
                break;
            }
            case TYPE_UDP:{
                udp_remove_listen_port(address->sin_port, internal_data->data.udp->listen_index);
                free(internal_data->address);
                break;
            }
        }
    }

    internal_data->address = address;
    internal_data->address_length = addr_length;


    switch(internal_data->data_type){
        case TYPE_TCP:{
            /* close the connection if necessary */
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
                        TH_FIN | TH_ACK, 
                        65535, 
                        0, 
                        0, 
                        NULL
                    );

                    while(internal_data->data.tcp->listeners[0].sequence_waiting_for_ack || !internal_data->data.tcp->listeners[0].is_fin){
                        time_count++;
                        kernel_sleep_us(1000);

                        if(!(time_count % TCP_TIME_RETRANSMISSION)){
                            break;
                        }
                    }

                    if(!internal_data->data.tcp->listeners[0].sequence_waiting_for_ack && internal_data->data.tcp->listeners[0].is_fin){
                        break;
                    }
                }

                spinlock_release(&internal_data->data.tcp->listeners[0].lock);   

                if(time_count >= TCP_TIME_OUT){
                    return EIO;
                }
            }

            internal_data->data.tcp->listen_index = tcp_listen_port(address->sin_port, &tcp_socket_handler, socket);

            internal_data->data.tcp->listen_allow_count = 0;
            internal_data->data.tcp->listen_accept_count = 1;
            internal_data->data.tcp->listeners = calloc(1, sizeof(socket_tcp_listener_t));

            internal_data->data.tcp->listeners[0].lock = (spinlock_t)SPINLOCK_INIT;
            internal_data->data.tcp->listeners[0].net_device = get_main_net_device();
            internal_data->data.tcp->listeners[0].ip_address = address->sin_addr.s_addr;
            internal_data->data.tcp->listeners[0].port = address->sin_port;
            
            internal_data->data.tcp->listeners[0].last_allocated = malloc(sizeof(tcp_listener_buffer_t));
            internal_data->data.tcp->listeners[0].last_allocated->size = 0;
            internal_data->data.tcp->listeners[0].last_read = malloc(sizeof(tcp_listener_buffer_t));
            internal_data->data.tcp->listeners[0].last_read->buffer = NULL;
            internal_data->data.tcp->listeners[0].last_read->next = internal_data->data.tcp->listeners[0].last_allocated;

            internal_data->data.tcp->listeners[0].internal_data_parent = internal_data;
           
    

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

                    spinlock_release(&internal_data->lock);
                    return 0;
                }else if(internal_data->data.tcp->listeners[0].is_reset){
                    spinlock_release(&internal_data->data.tcp->listeners[0].lock);

                    spinlock_release(&internal_data->lock);
                    return ECONNREFUSED;                    
                }
            }

            spinlock_release(&internal_data->data.tcp->listeners[0].lock);

            spinlock_release(&internal_data->lock);
            return ETIMEDOUT;
        }
        case TYPE_UDP:{
            internal_data->data.udp->listen_index = udp_listen_port(address->sin_port, &udp_socket_handler, socket);
            spinlock_release(&internal_data->lock);
            return 0;
        }
        default:
            break;
    }

    spinlock_release(&internal_data->lock);

    return 0;
}

int socket_send_handler(kernel_socket_t* socket, struct msghdr* hdr, int flags, size_t* size){
    // TODO do not ignore some hdr fields and flags
    if(vmm_check_memory(vmm_get_current_space(), (memory_range_t){(void*)hdr, sizeof(struct msghdr)})){
        return EINVAL;
    }

    if(vmm_check_memory(vmm_get_current_space(), (memory_range_t){(void*)hdr->msg_iov, sizeof(struct iovec)})){
        return EINVAL;
    }

    struct sockaddr* addr_ptr = hdr->msg_name;
    socklen_t addr_length = hdr->msg_namelen;

    return socket_general_send_handler(hdr->msg_iov->iov_base, hdr->msg_iov->iov_len, size, socket, addr_ptr, addr_length, flags);
}

int socket_recv_handler(kernel_socket_t* socket, struct msghdr* hdr, int flags, size_t* size){
    // TODO do not ignore some hdr fields and flags
    if(vmm_check_memory(vmm_get_current_space(), (memory_range_t){(void*)hdr, sizeof(struct msghdr)})){
        return EINVAL;
    }

    if(vmm_check_memory(vmm_get_current_space(), (memory_range_t){(void*)hdr->msg_iov, sizeof(struct iovec)})){
        return EINVAL;
    }

    if(vmm_check_memory(vmm_get_current_space(), (memory_range_t){(void*)hdr->msg_iov->iov_base, hdr->msg_iov->iov_len})){
        return EINVAL;
    }

    struct sockaddr* addr_ptr = hdr->msg_name;
    socklen_t addr_length = hdr->msg_namelen;

    return socket_general_receive_handler(hdr->msg_iov->iov_base, hdr->msg_iov->iov_len, size, socket, addr_ptr, addr_length, flags);
}

int socket_get_event(struct kernel_socket_t* socket, short events, short* revents){
    socket_internal_data_t* internal_data = (socket_internal_data_t*)socket->internal_data;

    *revents = internal_data->events & events;

    int events_count = internal_data->events_count;

    switch (internal_data->data_type){
        case TYPE_TCP_CHILD:{
            socket_tcp_listener_t* listener = internal_data->data.tcp_child->listener;
            if((listener->last_read->next->size > 0) && (events & POLLIN)){
                *revents |= POLLIN;
                events_count++;
            }
            return 0;
        }   
        case TYPE_TCP:{
            if(internal_data->data.tcp->is_connect){
                socket_tcp_listener_t* listener = &internal_data->data.tcp->listeners[0];
                if((listener->last_read->next->size > 0) && (events & POLLIN)){
                    *revents |= POLLIN;
                    events_count++;
                }
            }
            break;
        }
    }

    return events_count;
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

    socket_internal_data_t* internal_data = (socket_internal_data_t*)socket->internal_data;

    if(internal_data->data_type != TYPE_TCP){
        *error = EOPNOTSUPP;
        return NULL;
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
    socket_child->socket_send = &socket_send_handler;
    socket_child->socket_recv = &socket_recv_handler;
    socket_child->socket_get_event = &socket_get_event;

    socket_internal_data_t* child_internal_data = (socket_internal_data_t*)socket_child->internal_data;
    child_internal_data->lock = (spinlock_t)SPINLOCK_INIT;

    child_internal_data->address = internal_data->address;
    child_internal_data->address_length = internal_data->address_length;

    switch(internal_data->data_type){
        case TYPE_TCP:
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
            child_internal_data->data.tcp_child->listener->internal_data_parent = child_internal_data;
            break;
        default:
            break;
    }

    child_internal_data->events = POLLOUT;
    child_internal_data->events_count = 1;

    *error = 0;
    return socket_child;
}

kernel_socket_t* socket_handler(int family, int type, int protocol, int* error){
    if(protocol == 0 || protocol == 6 || protocol == 17){
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
        socket->stat = &socket_stat_handler;
        socket->close = &socket_close_handler;
        socket->socket_send = &socket_send_handler;
        socket->socket_recv = &socket_recv_handler;
        socket->socket_get_event = &socket_get_event;

        socket_internal_data_t* internal_data = (socket_internal_data_t*)socket->internal_data;
        internal_data->lock = (spinlock_t)SPINLOCK_INIT;

        switch(socket->type){
            case SOCK_STREAM:
                internal_data->data_type = TYPE_TCP;
                internal_data->data.tcp = calloc(1, sizeof(socket_tcp_data_t));
                break;
            case SOCK_DGRAM:
                internal_data->data_type = TYPE_UDP;
                internal_data->data.udp = calloc(1, sizeof(socket_udp_data_t));
                internal_data->data.udp->lock = (spinlock_t)SPINLOCK_INIT;
                internal_data->data.udp->cache_buffer = malloc(UDP_CACHE_SIZE);
                internal_data->data.udp->cache_size = UDP_CACHE_SIZE;
                internal_data->data.udp->cache_size_used = 0;
                break;
            default:
                break;
        }

        internal_data->events = POLLOUT;
        internal_data->events_count = 1;

        *error = 0;
        return socket;
    }else{
        *error = EPROTONOSUPPORT;
        return NULL;
    }
}

int socket_pair_handler(int family, int type, int protocol, kernel_socket_t** first_socket, kernel_socket_t** second_socket){
    int error;
    *first_socket = socket_handler(family, type, protocol, &error);

    if(!error){
        *second_socket = socket_handler(family, type, protocol, &error);
        if(error){
            (*first_socket)->close(*first_socket);
        }
    }else{
        (*first_socket)->close(*first_socket);
    }

    return error;
}

int socket_init(void){
    return s_add_family_handler(AF_INET, &socket_handler, &socket_pair_handler);
}