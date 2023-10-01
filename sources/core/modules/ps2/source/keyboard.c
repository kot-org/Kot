void keyboard_interrupt_handler(uint8_t data){
    
}

int keyboard_initialize(void){
    for(uint8_t i = 0; i < PS2_PORT_COUNT; i++){
        if(ps2_ports[i].type == PS2_TYPE_KEYBOARD && ps2_ports[i].is_present){
            log_printf("[module/"MODULE_NAME"] keyboard device found\n");
            
            hw_interrupt_set_handler(IRQ_START + ps2_ports[i].irq, &ps2_interrupt_handler);

            ps2_ports[i].interrupt_handler = &keyboard_interrupt_handler;

            /* set scan code 2 */
            ps2_send_data_port(ps2_ports, 0xf0);
            ps2_get_data();
            ps2_send_data_port(ps2_ports, 0x2);
            assert(ps2_get_data() == 0xfa);
            break;
        }
    }
    
    return 0;
}