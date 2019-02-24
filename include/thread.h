#ifndef THREAD_H
#define THREAD_H

#include <stdint.h>
#include <stddef.h>
#include "segment.h"

typedef struct thread thread;

#define THREAD_STOPPED 0x01 // Exited
#define THREAD_PAUSED  0x02 // Pause requested
#define THREAD_LOCKED  0x04 // Loked by an

struct thread
{
    const char* name;
    uint8_t flags;
    // Stack segment is personal for each thread, and should be stored
    size_t si;
    segment* stack;

    segment* section_begin;
    segment* section_end;
};

void switch_threads(); // Called by an interrupt, address should be given with knowlege of mapping

#endif // THREAD_H
