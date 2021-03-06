#ifndef __STRING_H
#define __STRING_H

#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

int         strcmp (const char*, const char*);
int         strncmp(const char*, const char*, size_t);
char*       strcpy (      char*, const char*);
char*       strncpy(      char*, const char*, size_t);
char*       strcat (      char*, const char*);
char*       strncat(      char*, const char*, size_t);
const char* strchr (const char*, char);
char*       strrev (      char*);
long        strtol (      char*, char**, int);
size_t      strlen (const char*);
size_t utf8_strlen (const uint32_t*);

int   atoi(const char*);
char* itoa(int, char*, int);
int   memcmp(const void*, const void*, size_t);
void* memmove(void* , void* , size_t);
void* memset(void*, uint8_t, size_t);
void* memcpy(void*, const void*, size_t);

int basreg(const char*, char***);

#ifdef __cplusplus
}
#endif

#endif