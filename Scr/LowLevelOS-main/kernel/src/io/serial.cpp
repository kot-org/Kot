#include "serial.h"
#define COM1 0x3f8

SerialPort* GlobalCOM1;

void SerialPort::Init() {
   outportb(COM1 + 1, 0x00);    // Disable all interrupts
   outportb(COM1 + 3, 0x80);    // Enable DLAB (set baud rate divisor)
   outportb(COM1 + 0, 0x03);    // Set divisor to 3 (lo byte) 38400 baud
   outportb(COM1 + 1, 0x00);    //                  (hi byte)
   outportb(COM1 + 3, 0x03);    // 8 bits, no parity, one stop bit
   outportb(COM1 + 2, 0xC7);    // Enable FIFO, clear them, with 14-byte threshold
   outportb(COM1 + 4, 0x0B);    // IRQs enabled, RTS/DSR set
   outportb(COM1 + 4, 0x0F);    // Set in normal mode
}

int SerialPort::isTransmitEmpty() {
    return inportb(COM1 + 5) & 0x20;
}

int SerialPort::isReceived() {
    return inportb(COM1 + 5) & 1;
}

char SerialPort::Read() {
    while(!isReceived());

    return inportb(COM1);
}

void SerialPort::Write(char chr) {
    while(!isTransmitEmpty());

    outportb(COM1, chr);
}

void SerialPort::Write(const char* chr) {
    for(int i = 0;chr[i] != '\0';i++){
        Write(chr[i]);
        if(chr[i] == '\n') Write('\r');   
    }
}

void SerialPort::ClearMonitor() {
    Write(27);
    Write("[2J");
    Write(27);
    Write("[H");
}

void SerialPort::Write(const char* chr,const char* chr2) {
    Write(chr);
    Write(chr2);
}

void SerialPort::Write(const char* chr,const char* chr2,const char* chr3) {
    Write(chr);
    Write(chr2);
    Write(chr3);
}