#include <stdint.h>
uint64_t __udivdi3(uint64_t a, uint64_t b)
{
    return (uint32_t) a / (uint32_t) b;
}
uint64_t __umoddi3(uint64_t a, uint64_t b)
{
    return (uint32_t) a % (uint32_t) b;
}
