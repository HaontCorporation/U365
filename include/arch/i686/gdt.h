#ifndef GDT_H
#define GDT_H

#include <stddef.h>
#include <stdint.h>

typedef struct gdt_entry_t gdt_entry_t;
typedef struct gdt_ptr_t   gdt_ptr_t;

/**
 * Defines a GDT entry.
 */

struct gdt_entry_t
{
    uint16_t limit_low;
    uint16_t base_low;
    uint8_t  base_middle;
    uint8_t  access;
    uint8_t  granularity;
    uint8_t  base_high;
} __attribute__((packed));

/**
 * Special pointer which includes the limit: The max bytes
 * taken up by the GDT, minus 1.
 */
struct gdt_ptr_t
{
    uint16_t limit;
    uint32_t base;
} __attribute__((packed));

/** Our GDT, with 3 entries, and finally our special GDT pointer */
/*struct gdt_entry gdt[3];
struct gdt_ptr gp;*/

extern gdt_entry_t* entries;
extern gdt_ptr_t    gdt_ptr;

/**
 * This will be a function in start.asm. We use this to properly
 * reload the new segment registers
 */
extern void gdt_flush();
void        gdt_install();
void        gdt_set_gate(int, uint32_t, uint32_t, uint8_t, uint8_t);

#endif
