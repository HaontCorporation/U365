#include <arch/i686/gdt.h>
#include <stdio.h>
#include <memory.h>

extern uint16_t gdt_start;
gdt_entry_t* gdt_entries;
gdt_ptr_t gdt_ptr;

//Code from Bran's tutorial
void gdt_set_gate(int num, uint32_t base, uint32_t limit, uint8_t access, uint8_t gran)
{
    gdt_entry_t* curr = gdt_entries + num;

    /* Setup the descriptor base address */
    curr->base_low = (base & 0xFFFF);
    curr->base_middle = (base >> 16) & 0xFF;
    curr->base_high = (base >> 24) & 0xFF;

    /* Setup the descriptor limits */
    curr->limit_low = (limit & 0xFFFF);
    curr->granularity = ((limit >> 16) & 0x0F);

    /* Finally, set up the granularity and access flags */
    curr->granularity |= (gran & 0xF0);
    curr->access = access;
}

/**
 * Should be called by main. This will setup the special GDT
 * pointer, set up the first 3 entries in our GDT, and then
 * finally call gdt_flush() in our assembler file in order
 * to tell the processor where the new GDT is and update the
 * new segment registers
 */
void gdt_install()
{
    void *gdt=malloc(sizeof(gdt_entry_t)*3 /* NULL, code, data */);
    gdt_entries=(gdt_entry_t*)gdt;
    /* Setup the GDT pointer and limit */
    gdt_ptr.limit = (uint16_t)0xFFFFFF;
    gdt_ptr.base = (uint32_t)(size_t)(gdt);

    /* Our NULL descriptor */
    gdt_set_gate(0, 0, 0, 0, 0);

/**
 * The second entry is our Code Segment. The base address
 * is 0, the limit is 4GBytes, it uses 4KByte granularity,
 * uses 32-bit opcodes, and is a Code Segment descriptor.
 */
    gdt_set_gate(1, 0, 0xFFFFFFFF, 0x9A, 0xCF);

/**
 * The third entry is our Data Segment. It's EXACTLY the
 * same as our code segment, but the descriptor type in
 * this entry's access byte says it's a Data Segment
 */
    gdt_set_gate(2, 0, 0xFFFFFFFF, 0x92, 0xCF);

    /* Flush out the old GDT and install the new changes! */
    gdt_flush();
}
