#include <stdint.h>
#include <stddef.h>
#include <time.h>
#include <stdarg.h>
#include <debug.h>
#include <stdio.h>
#include <memory.h>

//Only printk.
uint8_t printk_on = 0;
uint8_t log_ready = 0;
struct kernel_log __kernel_log;

void init_log()
{
	__kernel_log._end = calloc(sizeof(kernel_log_message), 1);
	__kernel_log._end->next = __kernel_log._end;
	__kernel_log._end->prev = __kernel_log._end;
	log_ready = 1;
}

void ins_log(kernel_log_message* mes)
{
	if(!log_ready)
		init_log();
	mes->next = __kernel_log._end;
	mes->prev = __kernel_log._end->prev;
	__kernel_log._end->prev->next = mes;
	__kernel_log._end->prev = mes;
}

void printk(char *format, ...)
{
	struct kernel_log_message *msg = calloc(sizeof(struct kernel_log_message), 1);
	msg->message = malloc(1024);
	msg->time = get_current_pit_time();
	va_list va;
	va_start(va, format);
	vsprintf(msg->message, format, va);
	ins_log(msg);
	if(printk_on)
	{
		tty_wrstr(msg->message);
		tty_wrstr("\n");
	}
	va_end(va);
}
