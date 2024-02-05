#include <core.h>

ps2_port_t ps2_ports[PS2_PORT_COUNT];

void interrupt_handler_stub(uint8_t data){
    log_success("stub\n");
}

void disable_ports(void){
    ps2_send_command(0xad); // disable port 1
    ps2_send_command(0xa7); // disable port 2
}

void enable_ports(void){
    ps2_send_command(0xae); // enable port 1
    ps2_send_command(0xa8); // enable port 2
}

int ports_initalize(void){
    ps2_ports[0].interrupt_handler = &interrupt_handler_stub;
    ps2_ports[1].interrupt_handler = &interrupt_handler_stub;

    disable_ports();

    /* test ps2 controller */
    ps2_send_command(0xaa);
    ps2_wait_output();
    if(ps2_get_data() != 0x55){
        return EIO;
    }

    ps2_send_command(0xab); // self test port 1
    ps2_wait_output();
    ps2_ports[0].is_present = ps2_get_data() == 0x0;

    ps2_send_command(0xa9); // self test port 2
    ps2_wait_output();
    ps2_ports[1].is_present = ps2_get_data() == 0x0;

    ps2_ports[0].type = PS2_TYPE_UNKNOW;
    ps2_ports[0].irq = PS2_IRQ_PORT1;
    ps2_ports[0].port_number = 0;

    ps2_ports[1].type = PS2_TYPE_UNKNOW;
    ps2_ports[1].irq = PS2_IRQ_PORT2;
    ps2_ports[1].port_number = 1;

    if(ps2_ports[0].is_present){
        log_printf("[module/"MODULE_NAME"] port 1 is present\n");

        ps2_send_command(0xae); // enable port 1  

        ps2_send_data_port1(0xf5);
        ps2_wait_output();
        ps2_get_data();
        ps2_send_data_port1(0xf2);
        ps2_wait_output();
        ps2_get_data();
        ps2_wait_output();
        uint8_t response0 = ps2_get_data();
        uint8_t response1 = ps2_get_data();
        switch(response0){
            case 0x0:
                ps2_ports[0].type = PS2_TYPE_MOUSE;
                break;
            case 0x3: 
                ps2_ports[0].type = PS2_TYPE_MOUSE_SCROLL;
                break;
            case 0x4:
                ps2_ports[0].type = PS2_TYPE_MOUSE_5BUTTONS;
                break;
            default:
                ps2_ports[0].type = PS2_TYPE_KEYBOARD;
                break;
        }  
        ps2_send_data_port1(0xf4);
        ps2_wait_output();
        ps2_get_data();
    }


    if(ps2_ports[1].is_present){
        log_printf("[module/"MODULE_NAME"] port 2 is present\n");

        ps2_send_command(0xa8); // enable port 2
        
        ps2_send_data_port2(0xf5);
        ps2_wait_output();
        ps2_get_data();
        ps2_send_data_port2(0xf2);
        ps2_wait_output();
        ps2_get_data();
        ps2_wait_output();
        uint8_t response0 = ps2_get_data();
        uint8_t response1 = ps2_get_data();
        switch(response0){
            case 0x0:
                ps2_ports[1].type = PS2_TYPE_MOUSE;
                break;
            case 0x3: 
                ps2_ports[1].type = PS2_TYPE_MOUSE_SCROLL;
                break;
            case 0x4:
                ps2_ports[1].type = PS2_TYPE_MOUSE_5BUTTONS;
                break;
            default:
                ps2_ports[1].type = PS2_TYPE_KEYBOARD;
                break;
        }  
        ps2_send_data_port2(0xf4);
        ps2_wait_output();
        ps2_get_data();
    }   

    uint8_t status = ps2_configuration_get();

    BIT_SETV(status, 0, ps2_ports[0].is_present);
    BIT_SETV(status, 4, !ps2_ports[0].is_present);
    
    BIT_SETV(status, 1, ps2_ports[1].is_present);
    BIT_SETV(status, 5, !ps2_ports[1].is_present);

    ps2_configuration_set(status);

    return 0;
}

int ps2_interrupt_handler(int id){
    if(ps2_get_status() & 0b1){
        int irq = id - IRQ_START;
        uint8_t data = (uint8_t)ps2_get_data();
        switch(irq){
            case PS2_IRQ_PORT1:
                ps2_ports[0].interrupt_handler(data);
                break;
            case PS2_IRQ_PORT2:
                ps2_ports[1].interrupt_handler(data);
                break;
        }
    }

    return 0;
}

void ps2_send_command(uint8_t command){
    io_write8(PS2_COMMAND, command);
    ps2_wait_input();
}


uint8_t ps2_get_status(void){
    return io_read8(PS2_STATUS);
}

void ps2_send_data(uint8_t data){
    io_write8(PS2_DATA, data);
    ps2_wait_input();
}


uint8_t ps2_get_data(void){
    return io_read8(PS2_DATA);
}

void ps2_wait_output(void){
    for(uint32_t timeout = 0; timeout < 0xfffff; timeout++){
        if(ps2_get_status() & 0b1){
            return;
        }
    }
    log_warning("%s : time out\n", __func__);
}

void ps2_wait_input(void){
    for(uint32_t timeout = 0; timeout < 0xfffff; timeout++){
        if(!(ps2_get_status() & 0b10)){
            return;
        }
    }
    log_warning("%s : time out\n", __func__);
}

void ps2_send_data_port1(uint8_t data){
    ps2_send_data(data);
    ps2_wait_input();
}

void ps2_send_data_port2(uint8_t data){
    ps2_send_command(0xd4);
    ps2_send_data(data);
    ps2_wait_input();
}

uint8_t ps2_configuration_get(void){
    ps2_send_command(0x20);
    ps2_wait_output();
    return ps2_get_data();
}

void ps2_configuration_set(uint8_t data){
    ps2_send_command(0x60);
    ps2_send_data(data);
}

uint8_t ps2_controller_output_get(void){
    ps2_send_command(0xd0);
    return ps2_get_data();
}

void ps2_controller_output_set(uint8_t data){
    ps2_send_command(0xd1);
    ps2_send_data(data);
}

void ps2_send_data_port(ps2_port_t* port, uint8_t data){
    if(port->port_number == 0){
        ps2_send_data_port1(data);
    }else if(port->port_number == 1){
        ps2_send_data_port2(data);
    }
}