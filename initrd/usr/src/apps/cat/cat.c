#include <stdio.h>
#include <string.h>

int main(int argc, char** argv)
{
	if(argc < 2)
	{
		fprintf(stderr, "cat: not enough arguments. Usage: cat <filenames>\n");
		return -1;
	}
	for(int i=1; i<argc; i++)
	{
		FILE* fp = fopen(argv[i], "r");
		if(!fp)
		{
            fprintf(stderr, "cat: failed to open file\n");
			return -1;
		}
		int16_t c;
		while((c = fgetc(fp)) != EOF)
			putchar((char) c);
		putchar('\n');
	}
	return 0;
}
