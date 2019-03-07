
#include <gdt.h>
#include <stdio.h>
#include <memory.h>

gdt_entry_t *gdt_entries;
gdt_ptr_t gdtr;

void gdt_set_gate(int num, uint32_t base, uint32_t limit, uint8_t access, uint8_t gran)
{
    gdt_entry_t *gdt_entry = gdt_entries + num;

    gdt_entry->base_low = (base & 0xFFFF);
    gdt_entry->base_middle = (base >> 16) & 0xFF;
    gdt_entry->base_high = (base >> 24) & 0xFF;
    gdt_entry->limit_low = limit & 0xFFFF;
    gdt_entry->granularity = ((limit >> 16) & 0x0F) | (gran & 0xF0);
    gdt_entry->access = access;
}

void gdt_install()
{
    gdt_entries = malloc(sizeof(gdt_entry_t) * 8);
    gdtr.limit = (sizeof(gdt_entry_t) * 8) - 1;
    gdtr.base = gdt_entries;

    gdt_set_gate(0, 0, 0, 0, 0);             // Null
    gdt_set_gate(1, 0, 0, 0x9A, 0x20);       // 64-bit Kernel Code
    gdt_set_gate(2, 0, 0, 0x92, 0x20);       // 64-bit Kernel Data
    gdt_set_gate(3, 0, 0, 0xFA, 0x20);       // 64-bit User Code
    gdt_set_gate(4, 0, 0, 0xF2, 0x20);       // 64-bit User Data
    gdt_set_gate(5, 0, 0xFFFFF, 0x9A, 0x80); // 16-bit Kernel Code
    gdt_set_gate(6, 0, 0xFFFFF, 0x92, 0x80); // 16-bit Kernel User
    //tss_set_gate(7);
    gdt_flush();
}