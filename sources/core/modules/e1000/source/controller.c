#include <controller.h>

/* General purpose */
void e1000_write_register(e1000_controller_t* controller, uint16_t reg, uint32_t value){
    if(controller->is_bar_io) {
        io_write32(controller->bar_base.io, reg);
        io_write32(controller->bar_base.io + (uint16_t)sizeof(uint32_t), value);
    }else{
        *((uint32_t*)((uintptr_t)controller->bar_base.memory + (uintptr_t)reg)) = value;
    }
}

uint32_t e1000_read_register(e1000_controller_t* controller, uint16_t reg){
    if(controller->is_bar_io) {
        io_write32(controller->bar_base.io , reg);
        return io_read32(controller->bar_base.io + (uint16_t)sizeof(uint32_t));
    }else{
        return *((uint32_t*)((uintptr_t)controller->bar_base.memory + (uintptr_t)reg));
    }
}

static inline uint32_t e1000_count_rx_descriptor(e1000_controller_t* controller, int head, int tail){
    if(tail <= head){
        return head - tail;
    }

    if(head < tail){
        return (controller->rx_desc_count + head) - tail;
    }

    return 0;
}

/* Interrupt */
e1000_controller_t* e1000_get_controller_from_interrupt_id(int id){
    for(size_t i = 0; i < e1000_controller_count; i++){
        if(e1000_controllers[i]->interrupt_vector == id){
            return e1000_controllers[i];
        }
    }
    return NULL;
}

int e1000_controller_on_interrupt(int id){
    e1000_controller_t* controller = e1000_get_controller_from_interrupt_id(id);

    assert(controller != NULL);

    if(e1000_read_register(controller, E1000_ICR) & E1000_ICR_RXQ0){
        int head = e1000_read_register(controller, E1000_RDH0);
        int tail = e1000_read_register(controller, E1000_RDT0);
        int rx_desc_count_to_handle = e1000_count_rx_descriptor(controller, head - 1, tail);
        int tail_current = (tail + 1) % controller->rx_desc_count;

        e1000_write_register(controller, E1000_ICR, E1000_ICR_RXQ0);

        for(int i = 0; i < rx_desc_count_to_handle; i++){
            net_handler->rx_packet(controller->net_device, controller->rx_desc[i + tail_current].length, vmm_get_virtual_address((void*)controller->rx_desc[i + tail_current].buffer_addr));

            /* Reset data */
            controller->rx_desc[i + tail_current].length = 0;
            controller->rx_desc[i + tail_current].csum = 0;
            controller->rx_desc[i + tail_current].status = 0;
            controller->rx_desc[i + tail_current].errors = 0;
            controller->rx_desc[i + tail_current].special = 0;
        }   
        e1000_write_register(controller, E1000_RDT0, (tail + rx_desc_count_to_handle) % controller->rx_desc_count);
    }

    return 0;
}

/* Specific functions */
int controller_send_packet(e1000_controller_t* controller, void* data, uint32_t size) {
    if(size > PACKET_SIZE){
        /* TODO */
        return EIO;
    }

    if(spinlock_acquire(&controller->tx_desc_lock)){
        return EIO;
    }

    uint8_t current_index = (uint8_t)e1000_read_register(controller, E1000_TDT);
    
    // wait the index to be usable
    while(!(controller->tx_desc[current_index].upper.fields.status & E1000_TXD_STAT_DD)){
        asm volatile("":::"memory");
    }

    void* virtual_address = vmm_get_virtual_address((void*)controller->tx_desc[current_index].buffer_addr);
    memcpy(virtual_address, data, size);

    controller->tx_desc[current_index].lower.flags.length = size;
    controller->tx_desc[current_index].upper.fields.status &= ~E1000_TXD_STAT_DD;

    e1000_write_register(controller, E1000_TDT, (current_index + 1) % controller->tx_desc_count);

    spinlock_release(&controller->tx_desc_lock);

    return 0;
}

/* Interface function */
int e1000_interface_tx_packet(struct net_device_t* net_device, size_t size, void* buffer){
    e1000_controller_t* controller = (e1000_controller_t*)net_device->internal_data;
    return controller_send_packet(controller, buffer, size);
}


/* Initialisation */
void controller_init_tx(e1000_controller_t* controller){
    controller->tx_desc_lock = (spinlock_t)SPINLOCK_INIT;
    controller->tx_desc_physical = pmm_allocate_page();
    controller->tx_desc_count = PAGE_SIZE / sizeof(struct e1000_tx_desc);
    controller->tx_desc = vmm_get_virtual_address(controller->tx_desc_physical);

    for(size_t i = 0; i < controller->tx_desc_count; i++) {
        controller->tx_desc[i].buffer_addr = (uint64_t) pmm_allocate_pages(PACKET_SIZE / PAGE_SIZE);
        controller->tx_desc[i].lower.flags.length = PACKET_SIZE;
        controller->tx_desc[i].lower.flags.cso = 0;
        controller->tx_desc[i].lower.flags.cmd = (uint8_t)(((uint32_t)(E1000_TXD_CMD_EOP | E1000_TXD_CMD_IFCS | E1000_TXD_CMD_RS)) >> 24); 
        controller->tx_desc[i].upper.fields.status = E1000_TXD_STAT_DD;
        controller->tx_desc[i].upper.fields.css = 0;
        controller->tx_desc[i].upper.fields.special = 0;
    }

    e1000_write_register(controller, E1000_TDBAL, (uint32_t) ((uint64_t)controller->tx_desc_physical & 0xFFFFFFFF));
    e1000_write_register(controller, E1000_TDBAL + sizeof(uint32_t), (uint32_t) ((uint64_t)controller->tx_desc_physical >> 32));
    
    e1000_write_register(controller, E1000_TDLEN, controller->tx_desc_count * sizeof(struct e1000_tx_desc));

    // index initialization
    e1000_write_register(controller, E1000_TDT, 0);
    e1000_write_register(controller, E1000_TDH, 0);

    // activate transmission
    e1000_write_register(controller, E1000_TCTL, e1000_read_register(controller, E1000_TCTL) | E1000_TCTL_EN | E1000_TCTL_PSP);
}

void controller_init_rx(e1000_controller_t* controller){
    controller->rx_desc_lock = (spinlock_t)SPINLOCK_INIT;
    controller->rx_desc_physical = pmm_allocate_page();
    controller->rx_desc_count = PAGE_SIZE / sizeof(struct e1000_rx_desc);
    controller->rx_desc = vmm_get_virtual_address(controller->rx_desc_physical);

    for(size_t i = 0; i < controller->rx_desc_count; i++) {
        controller->rx_desc[i].buffer_addr = (uint64_t) pmm_allocate_pages(PACKET_SIZE / PAGE_SIZE);
        controller->rx_desc[i].length = 0;
        controller->rx_desc[i].csum = 0;
        controller->rx_desc[i].status = 0;
        controller->rx_desc[i].errors = 0;
        controller->rx_desc[i].special = 0;
    }

    e1000_write_register(controller, E1000_RDBAL0, (uint32_t) ((uint64_t)controller->rx_desc_physical & 0xFFFFFFFF));
    e1000_write_register(controller, E1000_RDBAL0 + sizeof(uint32_t), (uint32_t) ((uint64_t)controller->rx_desc_physical >> 32));
    
    e1000_write_register(controller, E1000_RDLEN0, controller->rx_desc_count * sizeof(struct e1000_tx_desc));

    // index initialization
    e1000_write_register(controller, E1000_RDT0, controller->rx_desc_count - 1);
    e1000_write_register(controller, E1000_RDH0, 0);

    // activate receive
    e1000_write_register(controller, E1000_RCTL, e1000_read_register(controller, E1000_RCTL) | E1000_RCTL_EN | E1000_RCTL_BAM);
}

e1000_controller_t* controller_init(pci_device_id_t device_id){
    e1000_controller_t* controller = (e1000_controller_t*)malloc(sizeof(e1000_controller_t));

    controller->net_device = (net_device_t*)malloc(sizeof(net_device_t));
    controller->net_device->internal_data = (void*)controller;

    pci_bar_info_t bar_info;

    assert(pci_handler->get_bar_device(device_id, 0, &bar_info) == 0);

    if(bar_info.type == PCI_BAR_TYPE_IO) {
        controller->is_bar_io = true;
        controller->bar_base.io = (uint16_t)(uintptr_t)bar_info.address;
    }else{
        controller->is_bar_io = false;
        controller->bar_base.memory = vmm_get_virtual_address(bar_info.address);
    }

    e1000_write_register(controller, E1000_CTRL, E1000_CTRL_RST);

    while(e1000_read_register(controller, E1000_CTRL) & E1000_CTRL_RST){
        asm volatile("":::"memory");
    }

    e1000_write_register(controller, E1000_CTRL, E1000_CTRL_ASDE | E1000_CTRL_SLU);

    controller->net_device->mac_address[0] = (uint8_t)(e1000_read_register(controller, E1000_RA) & 0xFF);
    controller->net_device->mac_address[1] = (uint8_t)(e1000_read_register(controller, E1000_RA) >> 8) & 0xFF;
    controller->net_device->mac_address[2] = (uint8_t)(e1000_read_register(controller, E1000_RA) >> 16) & 0xFF;
    controller->net_device->mac_address[3] = (uint8_t)(e1000_read_register(controller, E1000_RA) >> 24) & 0xFF;
    controller->net_device->mac_address[4] = (uint8_t)(e1000_read_register(controller, E1000_RA + sizeof(uint32_t)) & 0xFF);
    controller->net_device->mac_address[5] = (uint8_t)(e1000_read_register(controller, E1000_RA + sizeof(uint32_t)) >> 8) & 0xFF;

    e1000_write_register(controller, E1000_RA + sizeof(uint32_t), e1000_read_register(controller, E1000_RA + sizeof(uint32_t)) | E1000_RAH_AV);

    /* initialize tx */
    controller_init_tx(controller);

    /* initialize rx */
    controller_init_rx(controller);

    int vector = hw_interrupt_allocate();
    assert(vector >= 0);

    controller->interrupt_vector = vector;
    
    assert(!pci_handler->bind_msi(device_id, vector, 0, 0, NULL));
    hw_interrupt_set_handler(vector, e1000_controller_on_interrupt);

    /* enable interrupt */
    e1000_write_register(controller, E1000_EITR, 0xffff);
    e1000_write_register(controller, E1000_IVAR, (E1000_IVAR_INT_ALLOC_VALID  << E1000_IVAR_RXQ0_SHIFT));
    e1000_write_register(controller, E1000_IMS, E1000_IMS_RXQ0);

    controller->net_device->tx_packet = e1000_interface_tx_packet;
    controller->net_device->packet_type = packet_type_ethernet;

    net_handler->add_net_device(controller->net_device);
    
    return controller;
}