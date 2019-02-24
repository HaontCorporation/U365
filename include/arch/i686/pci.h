#ifndef PCI_H
#define PCI_H
#include <stdint.h>
#define CLASS_DEVICE_TOO_OLD 0x00 //device doesn't have class id, maybe it was made before its definition?
#define CLASS_MASS_STORAGE 0x01
#define CLASS_NETWORK_CTRLR 0x02
#define CLASS_DISPLAY 0x03
#define CLASS_MULTIMEDIA 0x04
#define CLASS_MEMORY 0x05
#define CLASS_BRIDGE 0x06
#define CLASS_SCC 0x07
#define CLASS_SYSTEM 0x08
#define CLASS_INPUT 0x09
#define CLASS_DOCK 0x0A
#define CLASS_PROCESSOR 0x0B
#define CLASS_SERIAL_BUS 0x0C
#define CLASS_WIRELESS 0x0D
#define CLASS_INTELLIGENTIO 0x0E
#define CLASS_SATELLITE /* wat? u365 doesn't support satellites -.- */ 0x0F
#define CLASS_ENCRYPT 0x10
#define CLASS_NO_DEVICE 0xFF

#define PCI_ADDRESS_PORT 0xCF8
#define PCI_DATA_PORT 0xCFC

#define PCI_VENDOR_NO_DEVICE 0xFFFF

typedef struct pci_header_00 {
	uint16_t vendor_id;
	uint16_t device_id;
	uint8_t  revision;
	uint8_t  prog_if;
	uint8_t  subclass_id;
	uint8_t  class_id;
	uint8_t  cache_line_size;
	uint8_t  latency_timer;
	uint8_t  hdr_type;
	uint8_t  bist;
	uint32_t bar[6];
	uint32_t cardbus_cis_ptr;
	uint16_t subsys_vendor;
	uint16_t subsys_id;
	uint32_t expansion_rom;
	uint8_t  capatibilities;
	uint8_t  reserved[3];
	uint32_t reserved2;
	uint8_t  int_line;
	uint8_t  int_pin;
	uint8_t  min_grant;
	uint8_t  max_latency;
} pci_header_00;
int pci_enumerate_to_screen();
void pci_find_device(uint16_t, uint16_t, uint8_t *, uint8_t *, uint8_t *);
uint32_t pci_get_bar(uint8_t, uint8_t, uint8_t, uint8_t, uint8_t, uint8_t *);
uint8_t pci_get_class(uint8_t, uint8_t, uint8_t);
uint8_t pci_get_subclass(uint8_t, uint8_t, uint8_t);
uint8_t pci_get_hdr_type(uint8_t, uint8_t, uint8_t);
uint16_t pci_get_vendor(uint8_t, uint8_t, uint8_t);
uint16_t pci_get_device(uint8_t, uint8_t, uint8_t);
const char *pci_get_device_type(uint8_t, uint8_t);
uint16_t pci_read_confspc_word(uint8_t, uint8_t, uint8_t, uint8_t);
#endif