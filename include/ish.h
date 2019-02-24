#ifndef ISH_H
#define ISH_H

#define MAX_COMMANDS 100

#include <stdint.h>
#include <stddef.h>

#include <memory.h>

typedef struct
{
	char name[128];
	char description[128];
	void (*action);
} command_table_t;
extern command_table_t cmds[MAX_COMMANDS];
extern int cmdNum;

// ish 2
extern size_t argsize;
int parse_noninteractive(const char*);
int ish2_main(int argc, char** argv);

#endif // ISH_H
