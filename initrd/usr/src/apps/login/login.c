#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define USER_RECORD_FIELD_COUNT 7
int num_entries = 0, user_count = 0;
char** entry_array;

/*
int authorize(int user_no)
{
	printf("Password: ");
	char* buf = malloc(512);
	gets(buf);
	if(!strcmp(entry_array[USER_RECORD_FIELD_COUNT*user_no+1], buf))
	{
		printf("Welcome to U365!\n");
		return 0;
	}
	printf("INCORRECT PASSWORD DIE");
	return 0;
}*/

int main()
{
	while(1)
	{
		FILE *fp = fopen("/etc/passwd", "r");
		if(!fp)
		{
			fprintf(stderr, "Login failed: failed to fopen /etc/passwd");
			return -1;
		}

		printf("Just an extra long greeting message to make it work\n");
		printf("login: ");
		char* buf = malloc(512);
		fgets(buf, 512, stdin);

		int c = 0, si = 0, cid = 0, id = -1;
		while((c = fgetc(fp)) != EOF)
		{
			if(((c == ':' || c == '\n') && !buf[si]) || (c == buf[si]))
			{
				++si;
			}
			else
			{
				si = 0;
				++cid;
				while(c != EOF && c != '\n')
					c = fgetc(fp);
			}
			if((c == ':' || c == '\n') && !buf[si])
			{
				id = cid;
				break;
			}
		}

		if(id != -1)
		{
			printf("User found\n");
			int lp = 0, lp2 = 0;
			while(c != '\n' && c != EOF)
			{
				lp2 = lp;
				lp = 0;
				while((c = fgetc(fp)) != EOF && c != '\n' && c != ':')
				{
					++lp;
				}
			}
			if(!lp2 || !lp)
			{
				printf("No fields\n");
				continue;
			}
			if(fseek(fp, -lp - lp2 - 1 - (c == '\n'), SEEK_CUR))
			{
				fprintf(stderr, "Failed to fseek!\n");
				continue;
			}
			c = 0;

			char* homedir = malloc(lp2 + 1);
			char* shell   = malloc(lp  + 1);
			fread(homedir, 1, lp2, fp);
			fseek(fp, 1, SEEK_CUR);
			fread(shell, 1, lp, fp);
            homedir[lp2] = 0;
			shell[lp] = 0;

			printf("\nHome: ");
            puts(homedir);
			printf("\nShell: ");
            puts(shell);
			putchar('\n');

            return 0;
			// RUN SHELL HERE
		}
		else
		{
			fprintf(stderr, "User not found\n");
		}
		fclose(fp);
	}

	return -1;
}
