#include <stdio.h>
int main(int argc __attribute__((unused)), char** argv __attribute__((unused)))
{
	puts("hello, world :)");
	while(1)
	{
		putchar(getchar());
	}
	return 0;
}