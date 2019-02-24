#include <arch/i686/acpi.h>

#include <stdio.h>
#include <time.h>
#include <debug.h>
#include <arch/i686/io.h>

struct FADT      *fadt    ; //Fixed ACPI Description Table.
struct DSDT_vals  dsdt    ; //Distributed System Description Table.
void             *dsdt_ptr;

void *findRSDP()
{
	printk("ACPI: searching RSDP");
	uint8_t *mem=(uint8_t*) 0x000E0000;
	while((unsigned int)mem!=0x000FFFFF)
	{
		if(memcmp(mem, "RSD PTR ", 8)==0)
		{
			uint8_t *bptr = (uint8_t *) mem;
			uint8_t check=0;
      		for (unsigned int i=0; i<sizeof(struct RSDPDescriptor); i++)
      		{
         		check += *bptr;
         		bptr++;
      		}
      		if(((struct RSDPDescriptor*) mem)->Revision>=1)
      		{
      			check=0;
      			uint8_t *bptr20 = (uint8_t *) mem;
      			for (unsigned int j=0; j<sizeof(struct RSDPDescriptor20); j++)
      			{
         			check += *bptr20;
         			bptr20++;
      			}
      		}
      		if(!check)
      		{
      			printk("Done, RSDP found on 0x%08x", mem);
				return mem;
			}
			else
			{
				printk("error: ACPI RSDP checksum is invalid.");
			}
		}
		mem++;
	}
	printk("RSDP not found.");
	return 0;
}
int acpi_check_hdr(uint8_t *table, const char *sig, int n)
{
	if(memcmp(table,sig,n)!=0)
		return 1;
	uint8_t checksum=0;
	//for(unsigned int i=0; i<sizeof(struct ACPISDTHeader); i++)
	//{
	//	checksum+=*table++;
	//}
	if(checksum==0)
		return 0;
	return 1;
}
void acpi_enable()
{
	printk("ACPI: Trying to enable ACPI...");
	if((inw(fadt->PM1aControlBlock) >> 1 & 1) == 0 )
	{
		printk("ACPI already enabled");
		return;
	}
	outb(fadt->SMI_CommandPort,fadt->AcpiEnable);
	int i;
	for( i=0; i<300; i++)
	{
		if((inw(fadt->PM1aControlBlock) &1) == 0)
			break;
		sleep(10);
	}
	if((inw(fadt->PM1aControlBlock) &1) != 0)
	{
		printk("Failed to enable ACPI.");
	}
	printk("ACPI: ACPI enabled!");
}
void parse_dsdt()
{
	if(fadt==0)
	{
		printk("ACPI: FADT not found!");
	}
	if(acpi_check_hdr((uint8_t*)(int)fadt->Dsdt,"DSDT",4)!=0)
	{
		printk("ACPI: Invalid DSDT signature!");
	}
	dsdt_ptr=(void*)fadt->Dsdt;
	dsdt.length=((struct ACPISDTHeader *) dsdt_ptr)->Length-sizeof(struct ACPISDTHeader);
	//Find the S5 object.
	int i;
	for(i=0; i<=dsdt.length; i++)
	{
		if(memcmp(dsdt_ptr+sizeof(struct ACPISDTHeader)+i,"_S5_",4)==0)
		{
			dsdt.S5_object=dsdt_ptr+i;
			break;
		}
	}
	if(i==dsdt.length)
	{
		printk("ACPI: No S5 object in DSDT.");
	}
}
void parse_acpi(void *mem)
{
	//First set up the tables.
	struct RSDPDescriptor *rsdp=mem;
	struct RSDT           *rsdt=(struct RSDT*)(rsdp->RsdtAddress);
	int i=0;
	//Then check the headers and print an errors if neccesary
	if(acpi_check_hdr((uint8_t*)(int)rsdt,"RSDT",4)!=0)
	{
		printk("ACPI: Invalid RSDT signature.");
	}
	while(acpi_check_hdr((uint8_t*)(&rsdt->PointerToOtherSDT)[i],"FACP",4)!=0 && i<=64)
		i++;
	if(i<64)
	{
		fadt=(struct FADT*)(&rsdt->PointerToOtherSDT)[i];
	}
	else
	{
		printk("ACPI: FADT not found!");
	}
	const char *pm_profile_strings[9] = {"unspecified", "desktop", "notebook", "mobile", "workstation", "enterprice server", "SOHO server", "appliance PC", "perfomance server"};
	printk("This PC's power management profile is %s", (fadt->PreferredPowerManagementProfile < 8) ? pm_profile_strings[fadt->PreferredPowerManagementProfile] : "unknown, field is in reserved value");
	//Then parse the DSDT and enable ACPI.
	parse_dsdt();
	acpi_enable();
}
void parse_acpi20(void *mem)
{
	//First set up the tables.
	struct RSDPDescriptor20 *rsdp=(struct RSDPDescriptor20*)mem;
	struct XSDT           *xsdt=(struct XSDT*)(int)(rsdp->XsdtAddress);
	int i=0;
	//Then check the headers and print an errors if neccesary
	while(acpi_check_hdr((uint8_t*)(int)(&xsdt->PointerToOtherSDT)[i],"FACP",4)!=0 && i<=64)
		i++;
	if(i<64)
	{
		fadt=(struct FADT*)(int)(&xsdt->PointerToOtherSDT)[i];
	}
	else
	{
		printk("ACPI: FADT not found!");
	}
	//Then parse the DSDT and enable ACPI.
	parse_dsdt();
	acpi_enable();
}
void init_acpi()
{
	struct RSDPDescriptor *rsdp=findRSDP();
	if(rsdp==0)
	{
		tty_putchar('\n');
		printk("No ACPI on this computer.");
	}
	if(rsdp->Revision>=1)
	{
		parse_acpi20((void*)rsdp);
		return;
	}
	parse_acpi((void*)rsdp);
}
