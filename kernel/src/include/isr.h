
#include <stdbool.h>
#include <stdint.h>
#include <stddef.h>

#ifndef ISR
#define ISR

typedef struct isr isr;
struct isr
{
    uint64_t rax, rbx, rcx, rdx, rsi, rdi, rbp; // General Purpose Registers
    uint64_t cr0, cr2, cr3; // Control Registers
    uint64_t r8, r9, r10, r11, r12, r13, r14, r15; // More General Purpose Registers
    uint64_t fs, gs; // Segment Selectors
    uint64_t interrupt_no, error_code; // Exception Info
    uint64_t rip, cs, rflags, rsp, ss; // Stored for returning to the current state of the kernel
} __attribute__((packed));

void fault_handler(isr *state);
void isr_install();

#endif