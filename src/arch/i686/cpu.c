#include <arch/i686/io.h>
#include <time.h>
#include <sys.h>

uint16_t cpuMHZ;

uint16_t getCPUFreq()
{
    if(__u365_init_done)
        return cpuMHZ;
    uint64_t tsc=rdtsc();
    sleep(1000);
    uint64_t tsc2=rdtsc();
    return (tsc2-tsc)/1000/1000;
}
