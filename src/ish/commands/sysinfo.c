#include <stdio.h>
#include <arch/i686/smbios.h>
#include <string.h>

int sysinfo(int argc,char** argv) //Displaying system information.
{
    if(argc==1)
        printf(
            "U365 system info\n"
            "CPU Frequency       %d Mhz\n"
            "BIOS Vendor         %s\n"
            "BIOS Version        %s\n"
            "BIOS Release Date   %s\n",
            getCPUFreq(),
            (smbios_detected) ? detectBIOSVendor() : "ERR: No SMBIOS",
            (smbios_detected) ? detectBIOSVersion() : "ERR: No SMBIOS",
            (smbios_detected) ? detectBIOSBDate() : "ERR: No SMBIOS");
    else
    for(int i=1; i<argc; i++)
    {
        if(!strcmp(argv[i],"--help"))
            printf("U365 Kernel\n"
                   "--help\t\tDisplay this message\n"
                   "freq\t\tDisplay CPU Frequency\n"
                   "bvndr\t\tDisplay BIOS vendor\n"
                   "bver\t\tDisplay BIOS version\n"
                   "brldt\t\tDisplay BIOS release date\n"
                   "<no args>\tDisplay all system data.");
        if(!strcmp(argv[i], "freq"))
                printf("CPU Frequency       %d Mhz\n",getCPUFreq());
        if(!strcmp(argv[i], "bvndr"))
                printf("BIOS Vendor         %s\n",(smbios_detected) ? detectBIOSVendor() : "ERR: No SMBIOS");
        if(!strcmp(argv[i], "bver"))
                printf("BIOS Version        %s\n",(smbios_detected) ? detectBIOSVersion() : "ERR: No SMBIOS");
        if(!strcmp(argv[i], "brldt"))
                printf("BIOS Release Date   %s\n",(smbios_detected) ? detectBIOSBDate() : "ERR: No SMBIOS");
    }
    return 0;
}