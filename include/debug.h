#ifndef __DEBUG_H
#define __DEBUG_H
#define quot(x) #x
#define assert(check) asm("nop")

/*
enum fatal_err   {
	ENOTCOMPAT              ,
	EACPI_RSDP_SIG          ,
	EACPI_RSDP_CHECKSUM     ,
	EACPI_RSDT_INVALID      ,
	EACPI_FADT_NOT_FOUND    ,
	EACPI_DSDT_INVALID      ,
	EACPI_DSDT_NOFADT       ,
	EACPI_ENABLE_ERROR      ,
	EACPI_RSDP_CHKSM_INVALID,
	EACPI_DSDT_NOS5         ,
	EBOOT_MAGIC_ERROR       ,
	ENO_INITRD              ,
	EKMAIN_EXITED           ,
	EINCOMPATIBLE_BOOTLOADER,
};

enum dbg_msg_lvl {NOTICE, INIT, DEBUG, SUCCESS, WARNING, FATAL, ERROR, PANIC};
extern struct debug_log_entry {
	int level;
	const char *message;
} debug_info_messages[4096];
extern unsigned int debug_info_msg_count;*/

#include <stdint.h>
typedef struct kernel_log_message kernel_log_message;
struct kernel_log_message
{
	struct kernel_log_message *prev;
	char *message;
	uint64_t time; //in milliseconds
	struct kernel_log_message *next;
};

struct kernel_log
{
	kernel_log_message* _end;
};

extern struct kernel_log __kernel_log;

void debug_print(int, const char*);
void fatal_error(int);
void printk(char *, ...);

extern uint8_t printk_on;

#endif
