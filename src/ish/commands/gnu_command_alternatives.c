#include <stdio.h>
#include <string.h>
#include <fio.h>
#include <errno.h>
#include <env.h>
#include <uname.h>
#include <mtab.h>

int cmd_cat(int argc, char** argv)
{
	//error(0, ENODEV, "can't read files, no filesystem available");
	//return 127+ENODEV;
	if(argc < 2)
	{
		perr("cat: filename(s) expected");
		return -1;
	}
	if(argc>=64)
	{
		perr("cat: too many input files");
		return -2;
	}
	FILE* fp;
	int i;
	errno = 0; //TODO: it needs to be nulled by libc
	for(i=1; i<argc; i++)
	{
		fp = fopen(argv[i], "r");
		if(fp)
		{
			char c;
			while(((int16_t)(c = fgetc(fp)))!=EOF)
				tty_putchar(c);
			fclose(fp);
		}
		else
		{
			printf("Failed to open file: %s: %s\n", argv[i], strerror(errno));
			return 127+errno;
		}
	}
	return 0;
}

int cmd_echo(int argc, char** argv)
{
	int i;
	for(i=1; i<argc; i++)
	{
		printf("%s",argv[i]);
		tty_putchar(' ');
	}
	tty_putchar('\n');
	return 0;
}
int cmd_ls(int argc, char** argv)
{
	vector *v = vector_new();
	ls_flags_t flags = 0;

	int i, j;
	for(i = 1; i < argc; ++i)
	{
		if(argv[i][0] == '-')
		{
			if(argv[i][1] == '-')
			{
				if(!strncmp(argv[i]+2, "color", 5))
				{
					if(strcmp(argv[i]+2, "color=never"))
						flags |= LS_COLOR;
					else
						flags &= ~(LS_COLOR);
				}
			}
			else
			{
				for(j = 1; argv[i][j]; ++j)
				{
					switch(argv[i][j])
					{
						case 'a': flags |= LS_POINTS; /* fall through */
						case 'A': flags |= LS_HIDDEN; break;
						case 'F': flags |= LS_SYMBOLS; break;
					}
				}
			}
		}
		else
		{
			vector_push_back(v, argv[i]);
		}
	}
	if(!vector_size(v))
	{
		vector_push_back(v, ".");
	}

	for(i = 0; i < (int)(vector_size(v)); ++i)
	{
		if(fs_list_files((const char*)*(vector_at(v, i)), flags) == -1)
			return 2;
	}
	return 0;
}

int cmd_cd(int argc, char **argv)
{
	//error(0, ENODEV, "failed to change working directory");
	//return 127+ENODEV;
	if(argc>2)
	{
		fprintf(stderr, "cd: error: too many arguments.\nUsage:\ncd or cd <dir>\n");
		return 127+ENOMEM;
	}
	const char* targ;
	if(argc == 2)
		targ = expand_path(argv[1]);
	else
		targ = get_env_variable("HOME");

	struct stat t;
	if(stat(targ, &t))
	{
		printf("Could not stat \"%s\"\n", targ);
		targ = 0;
	}
	if(targ && !S_ISDIR(t.st_mode))
	{
		printf("\"%s\": not a directory");
		targ = 0;
	}

	if(!targ && argc < 2)
		targ = "/";

	if(targ)
	{
		set_env_variable("PWD", targ);
		return 0;
	}
	return 127 + ENODEV;
}

int cmd_pwd()
{
	printf("%s\n", get_env_variable("PWD"));
	return 0;
}
int cmd_uname(int argc, char** argv)
{
	char* help_text =\
	"Usage: uname [OPTION]...\n"
	"Print certain system information. With no OPTION, same as -s.\n\n"
	"  -a, --all\t\t\t\t print all information, in the following order:\n"
	"  -s, --kernel-name\t\t print the kernel name\n"
	"  -n, --nodename\t\t print the network node hostname\n"
	"  -r, --kernel-release\t print the kernel release\n"
	"  -v, --kernel-version\t print the kernel version\n"
	"  -m, --machine\t\t\t print the machine hardware name\n"
	"  -o, --operating-system print the operating system";
	struct utsname info = get_uname();
	if(argc < 2)
	{
		printf("%s\n", info.system_name, argv);
		return 0;
	}
	int a_displayed=0,
		s_displayed=0,
		n_displayed=0,
		r_displayed=0,
		v_displayed=0,
		m_displayed=0,
		o_displayed=0;
	for(int i=1; i<argc; i++)
	{
		if(strcmp(argv[i], "--help") == 0)
		{
			printf("%s\n", help_text);
			return 0;
		}
		if(argv[i][0] == '-')
			for(unsigned j=1; j<strlen(argv[i]); j++)
				switch(argv[i][j])
				{
					case 'a':
						if(!a_displayed)
						{
							printf("%s %s %s %s %s %s\n",
									info.system_name,
									info.hostname,
									info.release,
									info.version,
									info.machine,
									info.os_name);
							return 0;
						}
						/* fall through */
					case 's':
						if(!s_displayed)
							printf("%s ", info.system_name);
						break;
					case 'n':
						if(!n_displayed)
							printf("%s ", info.hostname);
						break;
					case 'r':
						if(!r_displayed)
							printf("%s ", info.release);
						break;
					case 'v':
						if(!v_displayed)
							printf("%s ", info.version);
						break;
					case 'm':
						if(!m_displayed)
							printf("%s ", info.machine);
						break;
					case 'o':
						if(!o_displayed)
							printf("%s ", info.os_name);
						break;
					default:
						fprintf(stderr, "uname: invalid option -- '%c'\nTry 'uname --help' for more information.", argv[i][j]);
						return 1;
				}
	}
	return 0;
}

int export_main(int argc, char** argv)
{
	if(argc > 2)
	{
		printf("Too many arguments\n");
		return -1;
	}
	if(argc < 2)
	{
		display_env();
		return 0;
	}

	int i, eqpos = -1;
	for(i = 0; argv[1][i]; ++i)
	{
		if(argv[1][i] == '=')
		{
			eqpos = i;
			break;
		}
	}
	if(eqpos == -1)
	{
		printf("Equation not found\n");
		return -1;
	}

	char buf[128];
	strncpy(buf, argv[1], eqpos);
	set_env_variable(buf, argv[1] + eqpos + 1);

	return 0;
}
