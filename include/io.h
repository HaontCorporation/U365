#ifndef __IO_H
#define __IO_H
#include <stdint.h>
#include <stdbool.h>
#define PIC1 0x20
#define PIC2 0xA0
#define ICW1 0x11
#define ICW4 0x01
#define PIC1		0x20		/* IO base address for master PIC */
#define PIC2		0xA0		/* IO base address for slave PIC */
#define PIC1_COMMAND	PIC1
#define PIC1_DATA	(PIC1+1)
#define PIC2_COMMAND	PIC2
#define PIC2_DATA	(PIC2+1)
uint8_t  inb(uint16_t port);                //Reads 1 byte from port.
uint16_t inw(uint16_t port);                //Reads 2 bytes from port.
uint32_t inl(uint16_t port);                //Reads 4 bytes from port.
void     outb(uint16_t port, uint8_t  val); //Outputs 1 byte to port.
void     outw(uint16_t port, uint16_t val); //Outputs 2 bytes to port.
void     outl(uint16_t port, uint32_t val); //Outputs 4 bytes to port.
uint64_t rdtsc();                           //Returns CPU uptime after last reset in ticks.
void init_pics(int pic1, int pic2);
 
void insl(unsigned short port, unsigned int buffer, unsigned long count);
#endif