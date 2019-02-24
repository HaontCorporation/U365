#include <arch/i686/rtc.h>
#include <stdio.h>
#include <time.h>
int cmd_date()
{
    read_rtc();
    printf("Current UTC date/time is:\n"
           "%02d:%02d:%02d\n"
           "%02d/%02d/%04d\n"
           "Current UNIX(tm) timestamp is %u.\n",hour,minute,second,day,month,year,time(NULL));
    return 0;
}