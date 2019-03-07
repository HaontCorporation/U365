
#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include <kbd.h>
#include <io.h>

void init_arch()
{
    gdt_install();
    idt_install();
    initVBE();
//
    //printf("Setting up PIT\n");
    //setupPIT();
    //printf("Setting up PS/2 Keyboard\n");
    //initKbdInt();
    //kbd_scancodes_setup();
    printf("Setting up SMBIOS\n");
    uint8_t *mem;
    for (mem = 0xF0000; mem < 0x100000; mem += 16)
    {
        if (memcmp(mem, "_SM_", 4) == 0)
        {
            int length = mem[5];
            int checksum = 0;
            for (int i = 0; i < length; i++)
                checksum += mem[i];
//
            if (checksum == 0) break;
        }
    }
//
    if (mem == 0x100000)
    {
        tty_wrstr("warning: SMBIOS not found!\n");
        goto skip_smbios;
    }
    //smb_ep = mem;
    printf("SMBIOS found at 0x%08X\n", mem);
    //smbios_detected = true;
    printf("Getting hardware info from SMBIOS\n");
    //machine_bios_vendor = detectBIOSVendor();
    //machine_bios_version = detectBIOSVersion();
    //machine_bios_bdate = detectBIOSBDate();
skip_smbios:
    //init_acpi();
    //setup_paging();
//
    asm volatile ("sti");
}