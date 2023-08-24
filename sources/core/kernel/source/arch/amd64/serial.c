#include <impl/serial.h>

#include <arch/include.h>
#include ARCH_INCLUDE(io.h)

#define COM1     0x3f8
#define BOCHSLOG 0xe9

int serial_init(void) {

    io_write8(COM1 + 1, 0x00); // Disable all interrupts
    io_write8(COM1 + 3, 0x80); // Enable DLAB (set baud rate divisor)
    io_write8(COM1 + 0, 0x03); // Set divisor to 3 (lo byte) 38400 baud
    io_write8(COM1 + 1, 0x00); //                  (hi byte)
    io_write8(COM1 + 3, 0x03); // 8 bits, no parity, one stop bit
    io_write8(COM1 + 2, 0xC7); // Enable FIFO, clear them, with 14-byte threshold
    io_write8(COM1 + 4, 0x0B); // IRQs enabled, RTS/DSR set
    io_write8(COM1 + 4, 0x1E); // Set in loopback mode, test the serial chip
    io_write8(COM1 + 0, 0xAE); // Send a test byte
    
    // Check that we received the same test byte we sent
    if(io_read8(COM1 + 0) != 0xAE) {
        return 1;
    }
    
    // If serial is not faulty set it in normal operation mode:
    // not-loopback with IRQs enabled and OUT#1 and OUT#2 bits enabled
    io_write8(COM1 + 4, 0x0F);
    return 0;

}

void serial_write(char chr) {
    io_write8(COM1 + 0, chr);
}
