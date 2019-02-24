#include <arch/i686/rtc.h>
#include <time.h>

const uint16_t	lmos[] = {0, 31, 60, 91, 121, 152, 182, 213, 244, 274, 305, 335};
const uint16_t	mos[] = {0, 31, 59, 90, 120, 151, 181, 212, 243, 273, 304, 334};
time_t time(time_t* returnPtr)
{
	read_rtc();
	uint32_t days=0;
	time_t   secs=0;
	uint32_t mon=0, curr_year=0;

	/* Calculate number of days. */
	mon = month - 1;
	curr_year = year - _TBIAS_YEAR;
	days  = Daysto32(curr_year, mon) - 1;
	days += 365 * curr_year;
	days += day;
	days -= _TBIAS_DAYS;

	/* Calculate number of seconds. */
	secs  = 3600 * hour;
	secs += 60 * minute;
	secs += second;

    secs += (days * (time_t)86400);
    if(returnPtr)
        *returnPtr = secs;
    return secs;
	return secs;
}

int rand_seed = 0;
int last_rand = 0;
void srand(int seed)
{
	rand_seed = seed;
}

int rand()
{
    last_rand = (last_rand * last_rand + rand_seed * rand_seed - last_rand * rand_seed) * (rand_seed + last_rand);
    return last_rand;
}
