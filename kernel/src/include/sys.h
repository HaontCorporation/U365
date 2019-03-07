#ifndef __SYS_H
#define __SYS_H

#include <stdbool.h>
#include <stdint.h>

void arch_init();
uint16_t getCPUFreq();
extern bool        __u365_init_done    ;
extern const char* machine_bios_vendor ;
extern const char* machine_bios_version;
extern const char* machine_bios_bdate  ;

int detect_cpu();

#endif