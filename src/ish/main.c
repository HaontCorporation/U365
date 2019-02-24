#include <ish.h>

#include <cmds.h>
#include <stdio.h>
#include <panic.h>
#include <env.h>
#include <arch/i686/pci.h>
#include <string.h>
#include <task.h>
#include <debug.h>
#include <graphics/turtle.h>
#include <math.h>
#include <elf_hdr.h>
#include <modules.h>
#include <vector.h>
#include <arch/i686/kbd.h>
#include <time.h>

/**
 * Possible command line parsing order:
 * 1) Replace ALL $VAR and $(VAR) with corresponding values from process' ENV
 * 2) Try to read the first command.
 * 3) If after first monolit name there is an '=', treat it as ENV replacement. There is also a possible issue: setting ENV var name cannot be got from another var
 * 4) If there is a ';', '\n' or any other command separator, set it it current process' ENV, otherwise continue reading the input
 * 5) if a command name found, read it and it's args, treating everything in outer braces as a single arg, ';' outside braces and not \-ded as a command separator
 */

command_table_t cmds[MAX_COMMANDS];
int cmdNum=0;
static int last_return_code=0;
void print_ps1()
{
	const char *ps1=get_env_variable("PS1");
	int i=0;
	while(ps1[i]!='\0')
	{
		if(ps1[i]!='\\')
			putchar(ps1[i]);
		else
		{
			switch(ps1[i+1])
			{
				case 'h': printf("%s", get_env_variable("HOSTNAME")); break;
				case 'u': printf("%s", get_env_variable("USER")); break;
				case '$': printf("%c", (strcmp(get_env_variable("UID"), "0")==0) ? '#' : '$' ); break;
				case 'w': printf("%s", get_env_variable("PWD")); break;
				case 'r': if(last_return_code) {tty_wrstr("\e[91m"); printf("% 2d ", last_return_code); tty_wrstr("\e[0m");} break;
			}
			i++;
		}
		i++;
	}
}

void addCmd(const char *name, const char *desc, int (*fcn)())
{
	if(cmdNum + 1 < MAX_COMMANDS)
	{
		strcpy(cmds[cmdNum].name,        name);
		strcpy(cmds[cmdNum].description, desc);
		cmds[cmdNum].action=fcn;
		++cmdNum;
	}
}

uint8_t startCmd(int argc, char **argv)
{
	if(!**argv)
	{
		return 0;
	}

	for(int i = 0; i < cmdNum; i++)
	{
		if(strcmp(cmds[i].name, argv[0]) == 0)
		{
			int (*cmd_run)(int,char**) = cmds[i].action;
			last_return_code = cmd_run(argc, argv);
			return 0;
		}
	}
	return 1;
}

// Need sin/cos
#ifdef MODULE_TURTLE
void turtle_test()
{
	turtle_set_target_surface(surface_screen);
	turtle_set_position(300, 300);
	for(int i = 0; i < 100000; ++i)
	{
		turtle_set_pen_color_raw(i % 256, (i + 64) % 256, (i + 128) % 256);
		turtle_turn_left(i % 10);
		turtle_move_forward(i % 10);
	}
}
#endif

void insert_commands()
{
	addCmd("",            "Empty command",                              emptyCmd);
	addCmd("help",        "Display known commands",                     help);
	addCmd("sysinfo",     "System info (only with SMBIOS)",             sysinfo);
	addCmd("panic",       "Panic",                                      gpf);
	addCmd("cat",         "Show contents of specified files",           cmd_cat);
	addCmd("echo",        "Output all passed arguments",                cmd_echo);
	addCmd("reboot",      "Reboot your PC.",                            cmd_reboot);
	addCmd("date",        "Show system clock date and time.",           cmd_date);
	addCmd("surface",     "Test the screen surface",                    surface_test);
	addCmd("gradient",    "Test the screen surface",                    gradient_test);
	addCmd("lscpu",       "CPU information. Code isn't mine!",          cmd_lscpu);
	//addCmd("testelf",     "Load ELF file",                              cmd_elf_test);
	addCmd("ls",          "List files in a directory",                  cmd_ls);
	addCmd("cd",          "Change working directory",                   cmd_cd);
	addCmd("pwd",         "Print working directory",                    cmd_pwd);
	addCmd("font-viewer", "View TTF font glyphs",                       font_viewer);
	addCmd("font-write",  "Draw text using TTF font",                   cmd_write_ttf_str);
	addCmd("lspci",       "List all PCI devices",                       pci_enumerate_to_screen);
	addCmd("uname",       "Show system information",                    cmd_uname);
	addCmd("lsdev",       "List all devices inserted into __devtab",    ls_dev);
	addCmd("mount",       "List all mounted devices",                   mount_main);
	addCmd("export",      "Environment",                                export_main);
	addCmd("tui_demo",    "TUI toolkit demo",                           tui_demo_app);
	addCmd("file-viewer", "TUI file viewer",                            tui_demo_fileviewer);
	addCmd("ish",         "Integrated Shell",                           ish2_main);
	addCmd("dmesg",
					   "Show all debug messages. As seen from name, it acts like dmesg in Linux.",
																	 cmd_print_debug_info);
#ifdef MODULE_MOUSE
	addCmd("pen",         "Drawing program",                            pen_main);
#endif
#ifdef MODULE_TURTLE
	addCmd("turtle",      "Test turtle-style drawing",                  &turtle_test);
#endif
}

// Returns
// -1 on error
// -2 if command needs to be continued
// n >= 0, where n is the first command found otherwise
// str[n] can be treated as command splitter, if not 0
// TODO: braces, comments

int find_command_end(const char* str)
{
	int i;
	uint8_t quote1 = 0, quote2 = 0, slash = 0, noex = 0, octotorp = 0;
	for(i = 0; str[i]; ++i)
	{
		noex = quote1 || quote2 || slash;
		if((str[i] == ';' || str[i] == '\n') && !noex)
		{
			return i;
		}
		if(str[i] == '\\' && !noex)
			slash = 1;
		else
			slash = 0;

		if(str[i] == '#' && !noex)
			octotorp = 1;

		if(str[i] == '\'' && !quote2 && !slash && !octotorp)
			quote1 = !quote1;
		if(str[i] == '"' && !quote1 && !slash && !octotorp)
			quote2 = !quote2;
	}
	if(quote1 || quote2)
		return -2;
	return i;
}


#define STR_MAX 256
int startScript(const char* path)
{
	FILE* str = fopen(path, "r");
	if(!str)
		return -1;
	char buf[STR_MAX];
	while(fgets(buf, STR_MAX, str))
	{
		if(parse_noninteractive(buf) != 1)
			return 0;
	}
	return 0;
}

// Returns 0 if the command is "exit" and so we should stop the execution,
// 1 otherwise
int prepare_to_execute(const char* str)
{
	//printf("PREPARE(%s)\n", str);
	int argc;
	char** argv=malloc(256);
	argc = basreg(str, &argv);

	if(!strcmp(argv[0], "exit"))
	{
		printf("exit\n");
		return 0;
	}
	if(startCmd(argc, argv) == 1)
	{
		char* envp[] = {NULL};
		if(run_elf_file(argv[0], argv+1, envp, argc))
		{
			if(startScript(argv[0]))
			{
				printf("ish: no such command: %s", argv[0]);
				last_return_code = 127;
			}
		}
	}
	printf("\n");
	return 1;
}

// valid1 - check if this symbol can be used as the first symbol of env variable name
uint8_t valid1(char in)
{
	return (in >= 'A' && in <= 'Z') || (in >= 'a' && in <= 'z') || (in == '_');
}
// valid2 - check if env variable name can contain this symbol
uint8_t valid2(char in)
{
	return (in >= 'A' && in <= 'Z') || (in >= 'a' && in <= 'z') || (in == '_') || (in >= '0' && in <= '9');
}

//Just substitute any unquoted $STRING or $(STRING) with a corresponding env variable

int ish2_envsubst(const char* in, char* out)
{
	int result = 0;
	uint8_t quote1 = 0, quote2 = 0, slash = 0, st = 0;
	int i, j, k = 0, oq = 0, n = strlen(in);
	char buf[128];
	for(i = 0, j = 0; in[i]; ++i, ++j)
	{
		if(in[i] == '$')
		{
			int fp;
			st = 1;
			for(k = i + 1; in[k]; ++k)
			{
				if(st == 1)
				{
					if(in[k] == '(')
						++oq;
					else
						st = 2;
				}
				if(st == 2)
				{
					if((((!quote1 && !quote2) || oq)) && valid1(in[k]))
					{
						buf[0] = in[k];
						buf[1] = 0;
						st = 3;
						fp = k;
						continue;
					}
					st = 0;
					break;
				}
				if(st == 3)
				{
					if(valid2(in[k]))
					{
						buf[k - fp] = in[k];
						buf[k - fp + 1] = 0;
					}
					else
						st = 4;
					if(k == n - 1 && !oq) // Skip to apply immediately and do not leave that state
						st = 6;
				}
				if(st == 4)
				{
					if(oq)
					{
						if(in[k] == ')')
							--oq;
						else
						{
							st = 0;
							break;
						}
					}
					else
						st = 5;
				}
				if(st >= 5)
				{
					out[j] = 0;
					const char* apply = get_env_variable(buf);
					if(apply)
					{
						strcat(out, apply);
						j += strlen(apply);
					}
					st -= 5;
					i = k;
					++result;
				}
			}
		}

		if(in[i] == '\'' && !quote2 && !slash)
			quote1 = !quote1;
		if(in[i] == '"' && !quote1 && !slash)
			quote2 = !quote2;
		if(in[i] == '\\' && !quote1 && !quote2)
			slash = !slash;
		else
			slash = 0;
		if(!st)
			out[j] = in[i];
	}

	out[j] = 0;
	return result;
}

int parse_noninteractive(const char* str)
{
	int last = 0;
	char devared[1024] = {0};
	ish2_envsubst(str, devared);
	const char* active = devared;
	uint8_t of = 0;
	while(*active)
	{
		active += of;
		of = 1;
		last = find_command_end(active);
		if(last < 0)
		{
			printf("Input ended unexpectedly, aborting\n");
			return -1;
		}
		char* buf = calloc(last + 1, 1);
		strncpy(buf, active, last);

		// Now, just execute the command we found
		if(prepare_to_execute(buf) == 0)
			return 0;
		// Finally, move to the next one
		active += last;
	}
	return 1;
}
void inschar(char* str, char c, int pos)
{
	str += pos;
	while(c)
	{
		char d = *str;
		*str = c;
		c = d;
		++str;
	}
}

void delchar(char* str, int pos)
{
	str += pos;
	while(*str)
	{
		*str = *(str + 1);
		++str;
	}
}

void tty_draw_cursor(int x, int y, color_rgba color)
{
	int width = 8;
	int height = 16;
	rect r = {x * width, y * height + 14, width, 2};
	surface_fill_rect(surface_screen, color, r);
}

void autocompletion(char* in, int pos, int st, int* cur, int* tsy, int* cl)
{
	int start = 0, end = 0, i, w = 0, len = strlen(in);
	while(end < pos)
	{
		start = end;
		end = find_command_end(in + start);
	}
	for(i = start; i < pos; ++i)
	{
		if(in[i] == ' ')
		{
			start = i;
			++w;
		}
	}
	if(!w)
	{
		int sugg = -1, num = 0;
		for(i = 0; i < cmdNum; ++i)
		{
			if(!strncmp(in + start, cmds[i].name, pos - start))
			{
				//printf("\nSuggested %s (%i)\n", cmds[i].name, st);
				++num;
				if(num == 1)
					sugg = i;
				else
				{
					if(st)
					{
						if(num == 2)
							printf("%s\n", cmds[sugg].name);
						printf("%s\n", cmds[i].name);
					}
					else
						return;
				}
			}
		}
		// Insert our
		if(num == 1)
		{
			int off = pos - start;
			int tlen = strlen(cmds[sugg].name) - off;
			memmove(in + pos, in + pos + tlen, len - pos + 1);
			memcpy (in + pos, cmds[sugg].name + off, tlen);
			printf("%s", in + pos);
			*cur += tlen;
			*cl  += tlen;
		}
		if(num > 1 && st)
		{
			print_ps1();
			printf("%s", in);
			*tsy = tty_y;
		}
	}
}

char* interactive_read(vector* hist)
{
	int n = vector_size(hist), i, ni, cursor = 0, sc = 0, cl = 0;
	int tsx = tty_x, tsy = tty_y;
	int limit = 200;
	int blinkt = 0;
	int autocompletion_st = 0;

	//if(!n || *(char*)*(vector_at(hist, n - 1)))
	{
		vector_push_back(hist, calloc(1, limit));
		++n;
	}
	i = n - 1;
	ni = i;
	kbd_event ke;

	char* curr = (char*)*(vector_at(hist, i));
	while(1)
	{
		if(!curr)
		{
			printf("[ISH][fixme] NULL history entry on pos [%i]\n", i);
			return 0;
		}
		sleep(1);
		ke = kbd_buffer_pop();
		tty_goToPos(tsx + cursor, tsy);
		if(ke.exists && !ke.release)
		{
			switch(ke.code)
			{
				case KEY_ENTER:
					tty_goToPos(tsx + cursor, tsy);
					tty_putchar(sc < cl ? curr[cursor] : ' ');
					tty_goToPos(tsx + cl, tsy);
					tty_putchar('\n');
					return curr;
				case KEY_ARROW_UP:
					if(ni)
						--ni;
					break;
				case KEY_ARROW_DOWN:
					if(ni < n - 1)
						++ni;
					break;
				case KEY_ARROW_LEFT:
					if(cursor)
						--cursor;
					break;
				case KEY_ARROW_RIGHT:
					if(cursor < cl)
						++cursor;
					break;
				case KEY_HOME:
					cursor = 0;
					break;
				case KEY_END:
					cursor = cl;
					break;

				case KEY_BACKSPACE:
				{
					if(cursor)
					{
						--cursor;
						--cl;
						tty_goToPos(tsx + cursor, tsy);
						delchar(curr, cursor);
						printf("%s ", curr + cursor);
					}
					break;
				}
				case KEY_DELETE:
				{
					if(cursor < cl)
					{
						--cl;
						delchar(curr, cursor);
						tty_goToPos(tsx + cursor, tsy);
						printf("%s ", curr + cursor);
					}
					break;
				}
				case KEY_TAB:
					autocompletion(curr, cursor, autocompletion_st, &cursor, &tsy, &cl);
					autocompletion_st = 1;
					break;

				case KEY_C:
					if(ke.lctrl || ke.rctrl)
					{
						tty_goToPos(tsx + cl, tsy);
						printf("^C\n");
						return 0;
					}
				/* fall through */
				default:
				{
					char tc = kbd_event_convert(ke);
					if(tc && cl < limit)
					{
						inschar(curr, tc, cursor);
						printf("%s", curr + cursor);
						++cursor;
						++cl;
					}
				}
			}
			if(ke.code != KEY_TAB)
				autocompletion_st = 0;

			// If we are moving through history, we need to carefully erase the last
			if(ni != i)
			{
				int lcl = cl;
				i = ni;
				curr = (char*)*(vector_at(hist, ni));
				cl = strlen(curr);
				tty_goToPos(tsx, tsy);
				printf(curr);
				for(int k = cl; k < lcl; ++k)
					tty_putchar(' ');
				cursor = cl;
			}
			tty_goToPos(tsx + cursor, tsy);
		}
		// Draw the cursor
		if(sc != cursor)
		{
			tty_goToPos(tsx + sc, tsy);
			tty_putchar(sc < cl ? curr[sc] : ' ');
		}
		tty_goToPos(tsx + cursor, tsy);
		tty_draw_cursor(tsx + cursor, tsy, blinkt++ > 500 ? color_gray : color_black);
		sc = cursor;
		if(blinkt > 1000)
			blinkt = 0;
	}
	return 0;
}

int ish2_main(int argc, char** argv)
{
	uint8_t is_interactive = 1;
	if(argc < 2)
		is_interactive = 1;

	if(!cmdNum)
		insert_commands();

	vector* hist = vector_new();
	vector* achist = vector_new();

	if(is_interactive)
	{
		startScript("/etc/ishrc");
		const char* preexec = get_env_variable("ISH_STARTUP");
		if(preexec)
		{
			parse_noninteractive(preexec);
		}
		while(1)
		{
			preexec = get_env_variable("ISH_PREEXEC");
			if(preexec)
			{
				parse_noninteractive(preexec);
			}
			print_ps1();

			int i, n = vector_size(hist);
			vector_resize(achist, n);
			for(i = 0; i < n; ++i)
			{
				strcpy((char*)*vector_at(achist, i), (char*)*vector_at(hist, i));
			}

			char* input = interactive_read(achist);
			if(input && *input)
			{
				if(!n || strcmp(input, (char*)*(vector_at(hist, n - 1))))
				{
					char* ins = calloc(strlen(input) + 1, 1);
					strcpy(ins, input);
					vector_push_back(hist, ins);
				}

				if(!parse_noninteractive(input))
					return 0;
			}
		}
	}
	else
	{
		size_t clen = -1;
		for(int i = 1; i < argc; ++i)
			clen += strlen(argv[i]) + 1;
		char* buf = (char*)(malloc(clen));
		char* last = buf;
		for(int i = 1; i < argc; ++i)
		{
			if(i)
			{
				*last = ' ';
				++last;
			}
			strcpy(last, argv[i]);
			last += strlen(argv[i]);
		}
		parse_noninteractive(buf);
	}
	return 0;
}

