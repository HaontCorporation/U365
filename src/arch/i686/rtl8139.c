//RTL8139 NIC driver.
#include <arch/i686/pci.h>
#include <arch/i686/io.h>
#include <arch/i686/idt.h>
#include <debug.h>
#define RTL8139_VEN 0x10EC
#define RTL8139_DEV 0x8139
struct ethernet_header {

};
static struct {
	uint16_t iobase;
	uint8_t mac[6];
	uint8_t receive_buffer[8192+16+1500];
} rtl8139_state;
void rtl8139_irq(regs *r __attribute__((unused)))
{
	printk("Got Ethernet packet!");
	outw(rtl8139_state.iobase + 0x3E, 0x01);
}
void rtl8139_init()
{
	printk("rtl8139: driver started");
	uint8_t bus = 0, slot = 0, function = 0;
	pci_find_device(RTL8139_VEN, RTL8139_DEV, &bus, &slot, &function);
	if(bus == 0xFF && slot == 0xFF && function == 0xFF)
	{
		printk("rtl8139: no NIC found!");
		return;
	}
	uint8_t hdrtype = pci_get_hdr_type(bus, slot, function);
	uint8_t bartype = 0;
	rtl8139_state.iobase = (uint16_t) pci_get_bar(hdrtype, bus, slot, function, 0, &bartype);
	printk("rtl8139: card found on %02u:%02u.%u; I/O base addr is 0x%04X", bus, slot, function, rtl8139_state.iobase);
	printk("rtl8139: resetting card");
	//Power on and reset
	outb(rtl8139_state.iobase + 0x52, 0);
	outb(rtl8139_state.iobase + 0x37, 0x10);
	//wait for reset to complete
	while(inb(rtl8139_state.iobase + 0x37) & 0x10) ; 
	printk("rtl8139: card reset done, reading MAC");
	uint8_t mac[6] = {};
	for(int i=0; i<6; i++)
		mac[i] = inb(rtl8139_state.iobase + i);
	printk("rtl8139: MAC address is %02X:%02X:%02X:%02X:%02X:%02X",
		mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
	//set RX buffer :)
	outl(rtl8139_state.iobase + 30, (uint32_t) &rtl8139_state.receive_buffer);
	//setting up interrupts
	outw(rtl8139_state.iobase + 0x3C,  0x5);
	outb(rtl8139_state.iobase + 0x37, 0x0C);
	outw(rtl8139_state.iobase + 0x44, 0xf | (1 << 7));
	uint8_t irq_line = (uint8_t) pci_read_confspc_word(bus, slot, function, 0x3C);
	printk("rtl8139: debug: card uses IRQ %d", irq_line);
	irq_install_handler(irq_line, rtl8139_irq);
}