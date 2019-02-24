#include <debug.h>
#include <stdio.h>

int cmd_print_debug_info()
{
	kernel_log_message *msg;
	for(msg = __kernel_log._end->next; msg != __kernel_log._end; msg = msg->next)
		printf("[ %f ] %s\n", (float) msg->time / 1000, msg->message);
	return 0;
}
