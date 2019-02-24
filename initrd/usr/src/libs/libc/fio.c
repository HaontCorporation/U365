#include <stdio.h>
#include <fio.h>
#include <stdint.h>
#include <stdarg.h>
#include <string.h>
#include <stdlib.h>
#include <syscall_wrappers.h>

// Open
FILE* fopen(const char* name, const char* mode)
{
	int fd = open(name, mode);
	if(fd == -1)
		return 0;
	FILE* first_free = &_iob[0];
	while(first_free->exists && first_free != &_iob[OPEN_MAX - 1])
		first_free++;
	if(first_free == &_iob[OPEN_MAX - 1])
		return NULL;
	fill_file_struct(first_free, fd);
	return first_free;
}

void fill_file_struct(FILE* fp, int fd)
{
	fp->fd = fd;
	fp->position = 0;
	fp->exists = 1;
}
int stat(const char* name, struct stat *st)
{
	return fstat(name, st);
}

int fseek(FILE* stream, off_t offset, int origin)
{
	if(!stream)
		return -1;
	return lseek(stream->fd, offset, origin);
}

void fclose(FILE* stream)
{
	if(stream)
		close(stream->fd);
}

// Get
int fgetc(FILE* fp)
{
	uint8_t c;
	if(read(fp->fd, &c, 1) != 1)
		return EOF;
	else
		return c;
}

char* fgets(char* buf, int size, FILE* fp)
{
	int i, c = 0;
	for(i = 0; i < size && (c = fgetc(fp)) != EOF && c != '\n'; ++i)
	{
		buf[i] = c;
	}
	buf[i] = 0;
	return i ? buf : 0;
}

// Put
int fputc(int c, FILE* fp)
{
	write(fp->fd, &c, 1);
	return 0;
}
int fputs(const char* s, FILE* fp)
{
	int i=0;
	while(*s && !fputc(*s++, fp))
	{
		i++;
	}
	return i;
}
// Don't ever use
size_t fread(void* buf, size_t len, size_t numblocks, FILE* fp)
{
	return (read(fp->fd, buf, len*numblocks) / len);
}

// Receiving functions that will do everything as needed
int64_t va_get_signed(int8_t mod, va_list* va)
{
	switch(mod)
	{
		case -2: return (signed char         )(va_arg(*va, int      )); break;
		case -1: return (signed short     int)(va_arg(*va, int      )); break;
		case  0: return (signed           int)(va_arg(*va, int      )); break;
		case  1: return (signed long      int)(va_arg(*va, long     )); break;
		case  2: return (signed long long int)(va_arg(*va, long long)); break;
		default: return 0;
	};
}

uint64_t va_get_unsigned(int8_t mod, va_list* va)
{
	switch(mod)
	{
		case -2: return (unsigned char         )(va_arg(*va, unsigned int      )); break;
		case -1: return (unsigned short     int)(va_arg(*va, unsigned int      )); break;
		case  0: return (unsigned           int)(va_arg(*va, unsigned int      )); break;
		case  1: return (unsigned long      int)(va_arg(*va, unsigned long     )); break;
		case  2: return (unsigned long long int)(va_arg(*va, unsigned long long)); break;
		default: return 0;
	};
}
// Internal functions for vfprintf/vsprintf

typedef struct formparams formparams;
struct formparams
{
	int radix;          // 2, 8, 10, 16
	int precision;      // number of digits after '.' in floats
	int minlen;
	int maxlen;

	uint8_t sign;       // If 1, argument is treated as signed, unsigned otherwise
	uint8_t plus;       // Extra '+' for positive values
	uint8_t uppercase;  // 0xdeadbeef or 0xDEADBEEF
	uint8_t align_left;
	char    filler;     // '0' or ' '
	char    point;      // '.' or ',' or what else?!
};

int itoa_formated(char* out, uint64_t in, formparams* params)
{
	int result = 0;
	uint8_t neg = 0;
	int i;

	if(params->sign && (int64_t) (in) < 0)
	{
		neg = 1;
		in = -(int64_t)(in);
	}
	if(params->maxlen < (int)(1 + (neg || params->plus)) || params->minlen > params->maxlen)
		return -1;

	if(!in)
		out[result++] = '0';

	while(in && result + (neg || params->plus) < params->maxlen)
	{
		out[result] = (in % params->radix);
		if(out[result] > 9)
			out[result] += (params->uppercase ? 'A' : 'a') - 10;
		else
			out[result] += '0';
		++result;
		in /= params->radix;
	}

	int padsize = params->minlen - (int)(result + (neg || params->plus));
	if(padsize < 0)
		padsize = 0;

	// Fill zeroes between digits and minus if needed
	if(!params->align_left && params->filler == '0')
		for(i = 0; i < padsize; ++i)
			out[result++] = '0';

	// Minus if needed
	if(neg)
		out[result++] = '-';
	if(!neg && params->plus)
		out[result++] = '+';

	// Fill with filler before minus if needed
	if(!params->align_left && params->filler && params->filler != '0')
		for(i = 0; i < padsize; ++i)
			out[result++] = params->filler;

	out[result] = '\0';

	strrev(out);

	// Fill with filler after all the symbols if needed
	if(params->align_left && params->filler)
		for(i = 0; i < padsize; ++i)
			out[result++] = params->filler;

	out[result] = '\0';

	return result;
}

int ftoa_formated(char* out, long double in, formparams* params)
{
	int result = 0;
	uint8_t neg = 0;
	int i;

	if(/*isnan(in)*/0)
	{
		if(params->maxlen < 4)
			return -1;
		out[0] = 'N';
		out[1] = 'a';
		out[2] = 'N';
		out[3] = 0;
		return 3;
	}

	if(in < 0)
	{
		neg = 1;
		in = -in;
	}

	if(/*isinf(in)*/0)
	{
		if(params->maxlen < (int)(4 + (neg || params->plus)))
			return -1;
		if(neg)
			out[result++] = '-';
		if(!neg && params->plus)
			out[result++] = '+';
		out[result++] = 'i';
		out[result++] = 'n';
		out[result++] = 'f';
		out[result] = 0;
		return result;
	}

	if(params->maxlen < 2 + params->precision + (neg || params->plus) || params->minlen > params->maxlen)
		return -1;

	uint64_t active = in;
	if(!active)
		out[result++] = '0';
	while(active && result + (neg || params->plus) < params->maxlen)
	{
		out[result] = (active % params->radix);
		if(out[result] > 9)
			out[result] += (params->uppercase ? 'A' : 'a') - 10;
		else
			out[result] += '0';
		++result;
		active /= params->radix;
	}

	int padsize = params->minlen - (int)(result + (neg || params->plus));
	if(padsize < 0)
		padsize = 0;

	// Fill zeroes between digits and minus if needed
	if(!params->align_left && params->filler == '0')
		for(i = 0; i < padsize; ++i)
			out[result++] = '0';

	// Minus if needed
	if(neg)
		out[result++] = '-';
	if(!neg && params->plus)
		out[result++] = '+';

	// Fill with filler before minus if needed
	if(!params->align_left && params->filler && params->filler != '0')
		for(i = 0; i < padsize; ++i)
			out[result++] = params->filler;

	out[result] = '\0';

	strrev(out);

	out[result++] = params->point;

	// Non-integer part
	in -= (int64_t)(in);
	for(i = 0; i < params->precision; ++i)
	{
		in *= params->radix;
		out[result] = in;
		in -= out[result];
		if(out[result] > 9)
			out[result] += (params->uppercase ? 'A' : 'a') - 10;
		else
			out[result] += '0';
		++result;
	}

	// Fill with filler after all the symbols if needed
	if(params->align_left && params->filler)
		for(i = 0; i < padsize; ++i)
			out[result++] = params->filler;

	out[result] = '\0';

	return result;
}
int vfprintf(FILE* stream, const char* fmt, va_list va)
{
	size_t i;
	size_t n = strlen(fmt);
	size_t j;
	int result = 0;

	for(i = 0; i < n; i++)
	{
		if(fmt[i] == '%')
		{
			formparams par;
			par.sign      = 0;
			par.plus      = 0;
			par.radix     = 10;
			par.uppercase = 0;
			par.minlen    = 0;
			par.precision = 0;
			par.minlen    = 0;
			par.maxlen    = 64;
			par.filler    = ' ';
			par.point     = '.';

			uint8_t  state      = 0; // will be set to 1 when data is ready and to 2 when data is put out
			uint8_t  flag_dot   = 0;
//			uint8_t  flag_space = 0;
			uint8_t  flag_zero  = 0;
			int8_t   modifiers  = 0;
			uint64_t out        = 0; // all of the integer types can be placed into uint64_t, so we'll use it to temporarily store the value
			uint8_t  ps         = 0;

			for(j = i + 1; j < n && !state; ++j)
			{
				switch(fmt[j])
				{
					// Weird
					case '%': fputc('%', stream); state = 2; break;
					// Flags
					case '.': flag_dot       = 1; break;
					case '-': par.align_left = 1; break;
					case '+': par.plus       = 1; break;
//					case ' ': flag_space     = 1; break;
					case '0': flag_zero      = 1; break;
					case 'l': if(modifiers >= 0 && modifiers <  2) ++modifiers; break;
					case 'h': if(modifiers <= 0 && modifiers > -2) --modifiers; break;
					case '*':
					{
						if(flag_dot)
						{
							par.precision = va_arg(va, int);
						}
						else
						{
							par.minlen = va_arg(va, int);
						}
						break;
					}

// Output types
					case 'c':
					{
						// character, passed as int
						fputc((char)(va_arg(va, int)), stream);
						state = 2;
						break;
					}
// String
					case 's':
					{
						const char* str;
						str = (const char*)(va_arg(va, const char*));
						if(str)
						{
							if(!ps)
								par.maxlen = INT32_MAX;
							int len = strlen(str);
							int padsize = par.minlen - len;
							if(padsize < 0)
								padsize = 0;
							if(!par.align_left)
								for(int k = 0; k < padsize; ++k)
									fputc(par.filler, stream);
							int k;
							for(k = 0; k < par.maxlen && str[k]; ++k)
								fputc(str[k], stream);
							result += k;
							if(par.align_left)
								for(k = 0; k < padsize; ++k)
									fputc(par.filler, stream);
						}
						state = 2;
						break;
					}
					case 'd': case 'i':
					{
						// signed decimal
						out       = va_get_signed(modifiers, &va);
						state     = 1;
						par.sign  = 1;
						par.radix = 10;
						break;
					}
					case 'u':
					{
						// unsigned decimal
						out       = va_get_unsigned(modifiers, &va);
						state     = 1;
						par.sign  = 0;
						par.radix = 10;
						break;
					}
					case 'o':
					{
						// unsigned octal
						out       = va_get_unsigned(modifiers, &va);
						state     = 1;
						par.sign  = 0;
						par.radix = 8;
						break;
					}
					case 'b':
					{
						// unsigned binary
						out       = va_get_unsigned(modifiers, &va);
						state     = 1;
						par.sign  = 0;
						par.radix = 2;
						break;
					}
					case 'x':
					{
						// unsigned lowercase hexadecimal
						out           = va_get_unsigned(modifiers, &va);
						state         = 1;
						par.sign      = 0;
						par.radix     = 16;
						par.uppercase = 0;
						break;
					}
					case 'X':
					{
						// unsigned uppercase hexadecimal
						out           = va_get_unsigned(modifiers, &va);
						state         = 1;
						par.sign      = 0;
						par.radix     = 16;
						par.uppercase = 1;
						break;
					}
// float
					case 'f':
					{
						long double r = 0;
						if(modifiers == 0)
							r = va_arg(va, double);
						if(modifiers == 1)
							r = va_arg(va, long double);
						if(modifiers && modifiers != 1)
							return -1;
						par.filler = flag_zero ? '0' : ' ';
						if(!flag_dot)
							par.precision = 6;
						char* buf = calloc(par.maxlen + 1, 1);
						int ol = ftoa_formated(buf, r, &par);
						if(ol == -1)
							return -1;
						result += ol;
						fputs(buf, stream);
						state = 2;
						free(buf);
						break;
					}
// change precision
					default:
					{
						if(fmt[j] >= '0' && fmt[j] <= '9')
						{
							if(flag_dot)
							{
								par.precision = par.precision * 10 + fmt[j] - '0';
							}
							else
							{
								par.minlen = par.minlen * 10 + fmt[j] - '0';
								ps = 1;
							}
						}
						else
						{
							fputc(fmt[j], stream);
						}
					}
				}
			}

			if(state)
				i = j - 1;

// Integer value here
			if(state == 1)
			{
				char* buf = calloc(par.maxlen + 1, 1);
				par.filler = flag_zero ? '0' : ' ';
				int ol = itoa_formated(buf, out, &par);
				if(ol == -1)
					return -1;
				fputs(buf, stream);
				result += ol;
				free(buf);
			}
		}
		else
		{
			fputc(fmt[i], stream);
		}
	}
	return result;
}
int fprintf(FILE *fp, const char* fmt, ...)
{
	va_list va;
	va_start(va, fmt);
	int result = vfprintf(fp, fmt, va);
	va_end(va);
	return result;
}
