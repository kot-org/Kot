#include <impl/serial.h>

// this is the qemu port!!
static volatile unsigned int * const UART0DR = (unsigned int *) 0x09000000;

int serial_init(void) {
    return 0;
}

void serial_write(char chr) {
    *UART0DR = chr;
}
