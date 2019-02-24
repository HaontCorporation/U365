#ifndef SEGMENT_H
#define SEGMENT_H

#include <stdint.h>
#include <stddef.h>

typedef struct segment segment;

#include "gdt.h"

struct segment
{
    void*   physoff; // Physical offset for MMU - wil be removed as soon as possibe
    size_t  virtoff;
    size_t  size;    //
    uint8_t privelege;
    uint8_t flags;     // ensure definition
    gdt dt;
};

#endif // SEGMENT_H
