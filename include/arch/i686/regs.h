#ifndef REGS_H
#define REGS_H
#include <stdint.h>
struct regs
{
    uint32_t gs,fs,es,ds;
    uint32_t edi,esi,ebp,esp,ebx,edx,ecx,eax;
    uint32_t int_no,err_code;
    uint32_t eip,cs,eflags,useresp,ss;
};
typedef struct regs      regs;
uint32_t read_eax();
uint32_t read_ebx();
uint32_t read_ecx();
uint32_t read_edx();
uint32_t read_esi();
uint32_t read_edi();

uint32_t read_cr0();
uint32_t read_cr1();
uint32_t read_cr2();
uint32_t read_cr3();
uint32_t read_eflags();
#endif