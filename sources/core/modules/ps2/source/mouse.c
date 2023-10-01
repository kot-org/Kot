void mouse_interrupt_handler(uint8_t data){

}

int mouse_initalize(void){
    for(uint8_t i = 0; i < PS2_PORT_COUNT; i++){
        if(ps2_ports[i].is_present){
            if(ps2_ports[i].type == PS2_TYPE_MOUSE
            || ps2_ports[i].type == PS2_TYPE_MOUSE_SCROLL
            || ps2_ports[i].type == PS2_TYPE_MOUSE_5BUTTONS){
                log_printf("[module/"MODULE_NAME"] mouse device found\n");
                
                hw_interrupt_set_handler(IRQ_START + ps2_ports[i].irq, &ps2_interrupt_handler);

                ps2_ports[i].interrupt_handler = &mouse_interrupt_handler;
            }
        }
    }
    
    return 0;
}