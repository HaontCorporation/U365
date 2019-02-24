//U365 libc's errno support code.
#include <stdio.h>
#include <memory.h>
#include <stdarg.h>

int errno=0;
const char* errno_strs[256] = { //256 for future
	"", //Errno should NEVER be zero, except for OS boot, when it's initialized to 0.
	"No such file or directory",
	"No such device or address",
	"Read-only filesystem",
	"Not enough space",
	"Permission denied",
	"Operation not permitted",
	"No such device",
};
const char* strerror(int err)
{
	return errno_strs[err];
}
void error(int status, int errnum, const char* format, ...)
{
	va_list va;
	va_start(va, format);
	//char* message = calloc(256, 1);
	char message[256];
	vsprintf(message, format, va);
	va_end(va);

	if(errnum)
		printf("error: %s: %s\n", message, strerror(errnum));
	else
		printf("error: %s\n", message);
	if(status) while(1);
}
