#pragma once

unsigned char inportb(unsigned int port);
unsigned long long inportl(unsigned long long port);
void outportb(unsigned int port,unsigned char value);
void outportw(unsigned short int port, unsigned short int data);
void outportl(unsigned long long port, unsigned long long data);
void io_wait();