#ifndef TIME_H
#define TIME_H

#include <stdint.h>
#include <stddef.h>

typedef int64_t time_t;

extern const uint16_t lmos[];
extern const uint16_t mos[];

#define Daysto32(year, mon) (((year - 1) / 4) + MONTAB(year)[mon])
#define _TBIAS_DAYS         ((70 * (uint32_t)365) + 17)
#define _TBIAS_SECS         (_TBIAS_DAYS * (time_t)86400)
#define _TBIAS_YEAR         1900
#define MONTAB(year)        ((((year) & 03) || ((year) == 0)) ? mos : lmos)

struct tm
{
	int tm_sec;  // seconds after the minute (0-61)
	int tm_min;  // minutes after the hour   (0-59)
	int tm_hour; // hours since midnight     (0-23)
	int tm_mday; // day of the month         (1-31)
	int tm_mon;  // months since January     (0-11)
	int tm_year; // years since 1900
	int tm_wday; // days since Sunday        (0-6)
	int tm_yday; // days since January 1     (0-365)

	// Unused, saved for standard compatibility
	int tm_isdst; // Daylight Saving Time flag
};


time_t time(time_t*);

extern time_t time_current;
extern time_t time_offset;

void sleep(size_t);
uint64_t get_current_pit_time(); //It's ONLY PIT TIMER TICKS. Don't think that if now it's milliseconds, it will be milliseconds forever, to get actual milliseconds we will create function pit_getTicksPerMs in future.

void set_time_current();
struct tm* localtime(const time_t* timer);
struct tm*    gmtime(const time_t* timer);
time_t          time(      time_t* timer);
time_t        mktime(struct tm   * timeptr);

void srand(int);
int rand();

#endif // TIME_H
