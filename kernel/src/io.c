#include <stdint.h>
/* Port access. */
uint8_t  inb(uint16_t port)
{
	uint8_t ret;
	asm volatile ( "in %0, %1" : "=a"(ret) : "Nd"(port) );
	return ret;
}
uint16_t inw(uint16_t port)
{
	uint16_t ret;
	asm volatile ( "in %0, %1" : "=a"(ret) : "Nd"(port) );
	return ret;
}
uint32_t inl(uint16_t port)
{
	uint32_t ret;
	asm volatile ( "in %0, %1" : "=a"(ret) : "Nd"(port) );
	return ret;
}
void     outb(uint16_t port, uint8_t  val)
{
	asm volatile ( "out %1, %0" : : "a"(val), "Nd"(port) );
}
void     outw(uint16_t port, uint16_t val)
{
	asm volatile ( "out %1, %0" : : "a"(val), "Nd"(port) );
}
void     outl(uint16_t port, uint32_t val)
{
	asm volatile ( "out %1, %0" : : "a"(val), "Nd"(port) );
}
uint64_t rdtsc()
{
    uint64_t ret;
    asm volatile ( "rdtsc" : "=A"(ret) );
    return ret;
}
void insl(unsigned short port, unsigned int buffer, unsigned long count)
{
asm("cld; rep; insd" :: "D" (buffer), "d" (port), "c" (count));
}

void outsb(unsigned short port, unsigned int buffer, unsigned long count)
{
asm("cld; rep; outsb" :: "D" (buffer), "d" (port), "c" (count));
}