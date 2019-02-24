//PCI support, yes!
#include <arch/i686/pci.h>
#include <arch/i686/io.h>
#include <stdint.h>
#include <stdio.h>
#include <debug.h>
//pci_read_confspc_word(): Returns 16-bit value from PCI Configuration Space of specified device at specified offset.
uint16_t pci_read_confspc_word(uint8_t bus, uint8_t slot, uint8_t function, uint8_t offset)
{
	uint32_t addr;
	uint32_t bus32 = bus;
	uint32_t slot32 = slot;
	uint32_t func32 = function;
	addr = (uint32_t)((bus32 << 16) | (slot32 << 11) |
           (func32 << 8) | (offset & 0xfc) | ((uint32_t)0x80000000)); //yes, this line is copied from osdev
	outl(PCI_ADDRESS_PORT, addr);
	return (uint16_t)((inl(PCI_DATA_PORT) >> ((offset & 2) * 8)) & 0xffff); //this too... I'm too lazy to write them
}
/*static const char *pci_class_id_strings[] = {
	"Old device, no class",
	"Mass Storage Controller",
	"Network Controller",
	"Display Controller",
	"Multimedia Controller",
	"Memory Controller",
	"PCI Bridge",
	"Simple Communication Controller",
	"System Device",
	"Input Device",
	"Docking Station",
	"CPU",
	"Serial Bus Controller",
	"Wireless Controller",
	"Intelligent I/O Controller",
	"Satellite Communication Controller (wut? I don't have satellites, lol)",
	"Encryption/Decryption Controller",
}; *//*
static const char *pci_subclass_id_strings[256][256] = {
	{"Unknown device", "Device without class", "VGA-compatible Controller"},
	{"Other Mass Storage Controller",
	 "SCSI Bus Controller", "IDE Controller", "Floppy Disk Controller",
     "IPI Bus Controller", "RAID Controller", "ATA Controller",
     "SATA Controller", "SCSI Controller"},
    {"Other Network Controller",
     "Ethernet Controller", "Token Ring Controller", "FDII Controller",
	 "ATM Controller", "ISDN Controller", "WorldFlip Controller",
	 "PICMG 2.14 Multi-Computing"},
	{"Unknown Video Device",
	 "VGA-compatible Controller", "XGA Controller", "3D Controller"},
	{"Unknown Multimedia Device",
	 "Video Device", "Audio Device", "Computer Telephony Device"},
	{"Unknown Memory Controller",
	 "RAM Controller", "Flash Controller"},
	{"Unknown PCI Bridge",
	 "Host Bridge", "ISA Bridge", "EISA Bridge",
	 "MCA Bridge", "PCI-to-PCI Bridge", "PCMCIA Bridge",
	 "NuBus Bridge", "CardBus Bridge", "RACEWay Bridge",
	 "PCI-to-PCI Bridge (Semi-Transparent)", "InfiniBand-to-PCI Host Bridge"},
	{"Unknown Communication Controller",
	 "Serial Controller", "Parallel Port", "Multiport Serial Controller",
	 "Modem", "IEEE 488.1/2 (GPIB) Controller", "Smart Card"},
	{"Unknown System Device",
	 "Programmable Interrupt Controller", "DMA Controller", "System Timer",
	 "Real-Time Clock", "Generic PCI Hot-Plug Controller"},
	{"Unknown Input Device",
	 "Keyboard Controller", "Digitizer", "Mouse Controller",
	 "Scanner Controller", "Gameport Controller"},
	{"Unknown Docking Station", "Generic Docking Station"},
	{"Unknown CPU", "i386 CPU", "i486 CPU", "Pentium CPU", "", "", "", "", "", "", "", "", "", "", "", "", "",
	 "Alpha CPU", "", "", "", "", "", "", "", "", "", "", "", "", "", //for padding, VERY bad
	 "PowerPC CPU", "", "", "", "", "", "", "", "", "", "", "", "", "",
	 "MIPS Processor", "", "", "", "", "", "", "", "", "", "", "", "", "",
	 "Co-processor"},
	{"Unknown Serial Bus Controller",
	 "FireWire Controller (IEEE 1394)", "ACCESS.bus", "SSA Controller",
	 "Universal Serial Bus Controller", "Fibre Channel Controller", "SMBus",
	 "InfiniBand Controller", "IPMI Interface", "SERCOS Interface",
	 "CANbus Controller"},
	{"Unknown Wireless Controller",
	 "Infrared Controller", "Consumer IR Controller", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "",
	 "RF Controller", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "",
	 "Bluetooth Controller", "Broadband Controller",
	 "802.11a (Wi-Fi) Controller", "802.11b (Wi-Fi) Controller"},
	{}, //AAAAAAA WHAT TO DO HERE
	{"Unknown Satellite Communication Controller", "", "TV Controller", "Audio Controller", "Voice Controller", "Data Controller"}
}; */
static struct {
	uint8_t dev_class, dev_subclass; //added dev_ to prevent errors in C++
	const char *name;
} pci_device_type_strings[] = {
	{0x00, 0x00, "Unknown device"},
	{0x00, 0x01, "VGA-compatible Device"},
	{0x01, 0x00, "SCSI Bus Controller"},
	{0x01, 0x01, "IDE Controller"},
	{0x01, 0x02, "Floppy Disk Controller"},
	{0x01, 0x03, "IPI Bus Controller"},
	{0x01, 0x04, "RAID Controller"},
	{0x01, 0x05, "ATA Controller"},
	{0x01, 0x06, "SATA Controller"},
	{0x01, 0x07, "Serial Attached SCSI Controller"},
	{0x01, 0x80, "Other Mass Storage Controller"},
	{0x02, 0x00, "Ethernet Controller"},
	{0x02, 0x01, "Token Ring Controller"},
	{0x02, 0x02, "FDDI Controller"},
	{0x02, 0x03, "ATM Controller"},
	{0x02, 0x04, "ISDN Controller"},
	{0x02, 0x05, "WorldFip Controller"},
	{0x02, 0x06, "PICMG 2.14 Multi Computing"},
	{0x02, 0x80, "Other Network Controller"},
	{0x03, 0x00, "VGA-compatible Controller"},
	{0x03, 0x01, "XGA Controller"},
	{0x03, 0x02, "3D Controller"},
	{0x03, 0x80, "Other Display Controller"},
	{0x04, 0x00, "Video Device"},
	{0x04, 0x01, "Audio Device"},
	{0x04, 0x02, "Computer Telephony Device"},
	{0x04, 0x80, "Other Multimedia Device"},
	{0x05, 0x00, "RAM Controller"},
	{0x05, 0x01, "Flash Controller"},
	{0x05, 0x80, "Other Memory Controller"},
	{0x06, 0x00, "Host Bridge"},
	{0x06, 0x01, "ISA Bridge"},
	{0x06, 0x02, "EISA Bridge"},
	{0x06, 0x03, "MCA Bridge"},
	{0x06, 0x04, "PCI-to-PCI Bridge"},
	{0x06, 0x05, "PCMCIA Bridge"},
	{0x06, 0x06, "NuBus Bridge"},
	{0x06, 0x07, "CardBus Bridge"},
	{0x06, 0x08, "RACEWay Bridge"},
	{0x06, 0x09, "PCI-to-PCI Bridge (Semi-Transparent)"},
	{0x06, 0x0A, "InfiniBand-to-PCI Host Bridge"},
	{0x06, 0x80, "Other Bridge Device"},
	{0x07, 0x00, "Serial Controller"},
	{0x07, 0x01, "Parallel Port"},
	{0x07, 0x02, "Multiport Serial Controller"},
	{0x07, 0x03, "Generic Modem"},
	{0x07, 0x04, "IEEE 488.1/2 (GPIB) Controller"},
	{0x07, 0x05, "Smart Card"},
	{0x07, 0x80, "Other Communication Device"},
	{0x08, 0x00, "Programmable Interrupt Controller"},
	{0x08, 0x01, "DMA Controller"},
	{0x08, 0x02, "System Timer"},
	{0x08, 0x03, "Real-Time Clock"},
	{0x08, 0x04, "Generic PCI Hot-Plug Controller"},
	{0x08, 0x80, "Other System Peripheral"},
	{0x09, 0x00, "Keyboard Controller"},
	{0x09, 0x01, "Digitizer"},
	{0x09, 0x02, "Mouse Controller"},
	{0x09, 0x03, "Scanner Controller"},
	{0x09, 0x04, "Gameport Controller"},
	{0x09, 0x80, "Other Input Controller"},
	{0x0A, 0x00, "Generic Docking Station"},
	{0x0A, 0x80, "Other Docking Station"},
	{0x0B, 0x00, "i386 CPU"},
	{0x0B, 0x01, "i486 CPU"},
	{0x0B, 0x02, "Pentium CPU"},
	{0x0B, 0x10, "Alpha CPU"},
	{0x0B, 0x20, "PowerPC CPU"},
	{0x0B, 0x30, "MIPS CPU"},
	{0x0B, 0x40, "Co-processor"},
	{0x0C, 0x00, "FireWire Controller"},
	{0x0C, 0x01, "ACCESS.bus Controller"},
	{0x0C, 0x02, "SSA Controller"},
	{0x0C, 0x03, "USB Controller"},
	{0x0C, 0x04, "Fibre Channel"},
	{0x0C, 0x05, "SMBus"},
	{0x0C, 0x06, "InfiniBand"},
	{0x0C, 0x07, "IPMI SMIC Interface"},
	{0x0C, 0x08, "SERCOS Interface"},
	{0x0C, 0x09, "CANbus Interface"},
	{0x0D, 0x00, "iRDA Compatible Controller"},
	{0x0D, 0x01, "Consumer IR Controller"},
	{0x0D, 0x10, "RF Controller"},
	{0x0D, 0x11, "Bluetooth Controller"},
	{0x0D, 0x12, "Broadband Controller"},
	{0x0D, 0x20, "802.11a (Wi-Fi) Ethernet Controller"},
	{0x0D, 0x21, "802.11b (Wi-Fi) Ethernet Controller"},
	{0x0D, 0x80, "Other Wireless Controller"},
	
	{0x00, 0x00, NULL} //here array ends
};
static struct {
	uint16_t vendor;
	const char *name;
} pci_vendor_name_strings[] = {
	{
		0x8086, "Intel Corporation"
	},
	{
		0x10DE, "NVIDIA"
	},
	{
		0x1002, "Advanced Micro Devices Inc."
	},
	{
		0x10EC, "Realtek Semiconductor Corp."
	},
	{
		0, NULL
	}
};
uint8_t pci_get_class(uint8_t bus, uint8_t slot, uint8_t function)
{
	return (uint8_t) (pci_read_confspc_word(bus, slot, function, 10) >> 8) /* shifting to leave only class id in the variable. */;
}
uint8_t pci_get_subclass(uint8_t bus, uint8_t slot, uint8_t function)
{
	return (uint8_t) pci_read_confspc_word(bus, slot, function, 10);
}
uint8_t pci_get_hdr_type(uint8_t bus, uint8_t slot, uint8_t function)
{
	return (uint8_t) pci_read_confspc_word(bus, slot, function, 7);
}
uint16_t pci_get_vendor(uint8_t bus, uint8_t slot, uint8_t function)
{
	return pci_read_confspc_word(bus, slot, function, 0);
}
uint16_t pci_get_device(uint8_t bus, uint8_t slot, uint8_t function)
{
	return pci_read_confspc_word(bus, slot, function, 2);
}
const char *pci_get_device_type(uint8_t dev_class, uint8_t dev_subclass)
{
	for(int i=0; pci_device_type_strings[i].name != NULL; i++)
		if(pci_device_type_strings[i].dev_class == dev_class && pci_device_type_strings[i].dev_subclass == dev_subclass)
			return pci_device_type_strings[i].name;
	return NULL;
}
const char *pci_get_vendor_name(uint16_t vendor)
{
	for(int i=0; pci_vendor_name_strings[i].name != NULL; i++)
		if(pci_vendor_name_strings[i].vendor == vendor)
			return pci_vendor_name_strings[i].name;
	return "unknown";
}
uint32_t pci_get_bar(uint8_t hdrtype, uint8_t bus, uint8_t slot, uint8_t func, uint8_t bar_number, uint8_t *bar_type)
{
	if((hdrtype & ~0x80) == 0)
	{
		uint8_t off = bar_number * 2;
		uint16_t bar_low  = pci_read_confspc_word(bus, slot, func, 0x10 + off);
		uint16_t bar_high = pci_read_confspc_word(bus, slot, func, 0x10 + off + 1);
		if((bar_low & 1) == 0)
		{
			if((bar_low & ~0b110) == 0x00) //32-bit bar, we don't support other :P
			{
				uint32_t ret = (uint32_t) bar_low | (uint32_t) (bar_high << 16);
				ret &= ~0b1111;
				*bar_type = 0;
				return ret;
			}
		}
		if((bar_low & 1) == 1)
		{
			uint32_t ret = (uint32_t) bar_low | (uint32_t) (bar_high << 16);
			ret &= ~0b11;
			*bar_type = 1;
			return ret;
		}
	}
	return 0;
}
void pci_find_device(uint16_t vendor, uint16_t device, uint8_t *bus_ret, uint8_t *slot_ret, uint8_t *func_ret)
{
	for(unsigned bus=0; bus<256; bus++)
		for(unsigned slot=0; slot<32; slot++)
			for(unsigned func=0; func<8; func++)
				if(pci_get_device(bus, slot, func) == device && pci_get_vendor(bus, slot, func) == vendor)
				{
					*bus_ret  = bus;
					*slot_ret = slot;
					*func_ret = func;
					return;
				}
	*bus_ret = *slot_ret = *func_ret = 0xFF;
}
int pci_enumerate_to_screen() //It's just a copy of pci_list, the only change is that it writes it to the screen, not to kernel log.
{
	uint8_t clid;
	uint8_t sclid;
	uint8_t hdrtype;
	uint16_t vendor;
	uint16_t device;
	for(unsigned bus = 0; bus < 256; bus++)
	{
		for(unsigned slot = 0; slot < 32; slot++)
		{
			unsigned func = 0;
			clid = pci_get_class(bus, slot, func);
			sclid = pci_get_subclass(bus, slot, func);
			vendor = pci_get_vendor(bus, slot, func);
			hdrtype = pci_get_hdr_type(bus, slot, func);
			device = pci_get_device(bus, slot, func);
			if(clid != 0xFF && vendor != 0xFFFF)
				printf("%02u:%02u.%u %s: %s, device: 0x%4x\n", bus, slot, func, pci_get_device_type(clid, sclid), pci_get_vendor_name(vendor), device);
			if((hdrtype & 0x80) == 0)
				for(func = 1; func < 8; func++)
				{
					clid = pci_get_class(bus, slot, func);
					sclid = pci_get_subclass(bus, slot, func);
					vendor = pci_get_vendor(bus, slot, func);
					device = pci_get_device(bus, slot, func);
					hdrtype = pci_get_hdr_type(bus, slot, func);
					if(clid != 0xFF && vendor != 0xFFFF)
						printf("%02u:%02u.%u %s: %s, device: 0x%4x\n", bus, slot, func, pci_get_device_type(clid, sclid), pci_get_vendor_name(vendor), device);
			}
		}
	}
	return 0;
}
void list_pci()
{
	uint8_t clid;
	uint8_t sclid;
	uint8_t hdrtype;
	uint16_t vendor;
	uint16_t device;
	printk("PCI device listing:");
	printk("-- START --");
	for(unsigned bus = 0; bus < 256; bus++)
	{
		for(unsigned slot = 0; slot < 32; slot++)
		{
			unsigned func = 0;
			clid = pci_get_class(bus, slot, func);
			sclid = pci_get_subclass(bus, slot, func);
			vendor = pci_get_vendor(bus, slot, func);
			hdrtype = pci_get_hdr_type(bus, slot, func);
			device = pci_get_device(bus, slot, func);
			if(clid != 0xFF && vendor != 0xFFFF)
				printk("%02u:%02u.%u %s: %s, device: 0x%4x", bus, slot, func, pci_get_device_type(clid, sclid), pci_get_vendor_name(vendor), device);
			if((hdrtype & 0x80) == 0)
				for(func = 1; func < 8; func++)
				{
					clid = pci_get_class(bus, slot, func);
					sclid = pci_get_subclass(bus, slot, func);
					vendor = pci_get_vendor(bus, slot, func);
					device = pci_get_device(bus, slot, func);
					hdrtype = pci_get_hdr_type(bus, slot, func);
					if(clid != 0xFF && vendor != 0xFFFF)
						printk("%02u:%02u.%u %s: %s, device: 0x%4x", bus, slot, func, pci_get_device_type(clid, sclid), pci_get_vendor_name(vendor), device);
			}
		}
	}
	printk("--  END  --");
	void rtl8139_init();
	rtl8139_init();
}