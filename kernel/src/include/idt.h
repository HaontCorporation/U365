#ifndef IDT
#define IDT

#include <stddef.h>
#include <stdint.h>
#include <regs.h>

typedef struct idt_entry idt_entry;
typedef struct idtr idtr;

/* Defines an IDT entry */
struct idt_entry
{
    uint16_t base_lo;
    uint16_t selector;
    uint8_t always0;
    uint8_t flags;
    uint64_t base_hi : 48;
    uint32_t reserved2;
} __attribute__((packed));

struct idtr
{
    uint16_t limit;
    uint64_t base;
} __attribute__((packed));

/* Declare an IDT of 256 entries. Although we will only use the
*  first 32 entries in this tutorial, the rest exists as a bit
*  of a trap. If any undefined IDT entry is hit, it normally
*  will cause an "Unhandled Interrupt" exception. Any descriptor
*  for which the 'presence' bit is cleared (0) will generate an
*  "Unhandled Interrupt" exception */
extern idt_entry idt[256];
extern idtr idtp;

/* This exists in 'start.asm', and is used to load our IDT */
void idt_set_gate(uint8_t num, uint64_t base, uint16_t sel, uint8_t flags);
void idt_install();

#endif