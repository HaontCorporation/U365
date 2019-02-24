#include <stdio.h>
#include <fio.h>
#include <time.h>
#include <arch/i686/kbd.h>
int cmd_reboot()
{
    fprintf(stderr, "\nSystem is rebooting in 5 seconds.\n");
    sleep(4150);
    fprintf(stderr, "THE SYSTEM IS GOING TO REBOOT NOW!\n");
    sleep(850);
    //Only kbd reboot is supported at this time.
    kbd_reset_cpu();
    return 0;
}