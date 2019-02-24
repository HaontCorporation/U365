#ifndef SMBIOS_H
#define SMBIOS_H

#include <stdbool.h>
#include <stdint.h>

typedef struct SMBIOSEntryPoint SMBIOSEntryPoint;
typedef struct SMBIOSHeader SMBIOSHeader;
typedef struct DateTime DateTime;

struct SMBIOSHeader
{
    uint8_t Type;
    uint8_t Length;
    uint16_t Handle;
};

struct SMBIOSEntryPoint
{
    char EntryPointString[4];    //This is _SM_
    uint8_t Checksum;              //This value summed with all the values of the table, should be 0 (overflow)
    uint8_t Length;                //Length of the Entry Point Table. Since version 2.1 of SMBIOS, this is 0x1F
    uint8_t MajorVersion;          //Major Version of SMBIOS
    uint8_t MinorVersion;          //Minor Version of SMBIOS
    uint16_t MaxStructureSize;     //Maximum size of a SMBIOS Structure (we will se later)
    uint8_t EntryPointRevision;    //...
    char FormattedArea[5];       //...
    char EntryPointString2[5];   //This is _DMI_
    uint8_t Checksum2;             //Checksum for values from EntryPointString2 to the end of table
    uint16_t TableLength;          //Length of the Table containing all the structures
    uint32_t TableAddress;	     //Address of the Table
    uint16_t NumberOfStructures;   //Number of structures in the table
    uint8_t BCDRevision;           //Unused
};
extern SMBIOSEntryPoint *smb_ep;

extern uint16_t cpuMHZ;
void detectCPUSpeed();
const char *detectBIOSVendor ();
const char *detectBIOSVersion();
const char *detectBIOSBDate  ();
extern bool smbios_detected;

#endif // SMBIOS_H