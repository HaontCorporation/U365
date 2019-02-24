#include <fio.h>
#include <env.h>

#include <string.h>
#include <memory.h>
#include <tty_old.h>
#include <stdio.h>
#include <fs.h>
#include <math.h>
#include <compare.h>
#include <errno.h>
#include <proctab.h>

int16_t fgetc(FILE* stream)
{
	if(stream)
	{
		if(stream->getc)
		{
			return stream->getc(stream);
		}
		else
		{

			perr("[STDIO]: File is not opened for reading\n");
			return -1;
		}
	}
	return -1;
}

int16_t fputc(char ch, FILE* stream)
{
	if(stream)
	{
		if(stream->putc)
		{
			return stream->putc(stream, ch);
		}
		else
		{
			perr("[STDIO]: File is not opened for writing\n");
			return -1;
		}
	}
	return -1;
}

size_t fgets(char* str, size_t num, FILE* stream)
{
	size_t result = 0;
	if(stream)
	{
		if(stream->gets)
		{
			result = stream->gets(stream, str, num);
		}
		else
		{
			if(stream->getc)
			{
				int16_t buf = 'b';
				while(result <= num && buf && buf != -1 && buf != '\n')
				{
					buf = fgetc(stream);
					if(buf != -1)
					{
						str[result++] = buf;
					}
				}
				if(str[result - 1])
				{
					str[result++] = 0;
				}
			}
			else
			{
				perr("[STDIO]: File is not opened for reading\n");
			}
		}
	}
	return result;
}

size_t fputs(const char* str, FILE* stream)
{
	size_t result = 0;
	if(stream)
	{
		if(stream->puts)
		{
			result = stream->puts(stream, str);
		}
		else
		{
			if(stream->putc)
			{
				int16_t buf;
				while(*str)
				{
					buf = fputc(str[result], stream);
					if(buf != -1)
					{
						++result;
					}
				}
			}
			else
			{
				if(stream != stderr) // If this error will occur while writing to stderr, we will have an infinite recursion
				{
					perr("[STDIO]: File is not opened for writing\n");
				}
			}
		}
	}
	return result;
}

int open(const char* path, int flags, mode_t mode)
{
	proctab_entry* ent = find_proc(c_pid);
	if(!ent)
		return -1;
	
	char* rpath = expand_path(path);
	dev_t dev;
	fs_interface* iface;
	const char* ln;
	if(mount_find(rpath, &ln, &dev, &iface))
	{
		errno = ENOENT;
		return -1;
	}
	FILE* fdev = open_dev(dev, "r");
	if(!fdev)
	{
		errno = ENODEV;
		return -1;
	}

	// Now, find and prepare the descriptor
	file_interface* fiface = 0;
	int id;
	for(id = 0; id < (int)(vector_size(ent->files)); ++id)
	{
		fiface = (file_interface*)*(vector_at(ent->files, id));
		if(!fiface) // Error, fix it
			*vector_at(ent->files, id) = calloc(sizeof(file_interface), 1);
		if(fiface->opened)
			fiface = 0;
		else
			break;
	}
	
	// If there were no free descriptors, create a new one
	if(!fiface)
	{
		fiface = calloc(sizeof(file_interface), 1);
		vector_push_back(ent->files, fiface);
	}
	memset(fiface, 0, sizeof(file_interface));

	// Now, fill it, if the file exists
	fs_open_file(fdev, iface, ln, flags, mode, fiface);
	if(!fiface->opened)
		return -1;
	
	return id;
}

FILE* fopen(const char* path, const char* mode)
{
	char* rpath = expand_path(path);
	dev_t dev;
	fs_interface* iface;
	const char* ln;
	if(mount_find(rpath, &ln, &dev, &iface))
	{
		errno = ENOENT;
		return 0;
	}
	FILE* fdev = open_dev(dev, "r");
	if(!fdev)
	{
		errno = ENODEV;
		return 0;
	}
	return fs_fopen_file(fdev, iface, ln, mode);
}

void freopen(const char* path, const char* mode, FILE* stream)
{
	fclose(stream);
	FILE* n = fopen(path, mode);
	memcpy(stream, n, sizeof(FILE));
}

void fclose(FILE* stream)
{
	if(stream)
	{
		if(stream->close)
			stream->close(stream);
		free(stream->data);
		memset(stream, 0, sizeof(FILE));
	}
}

int fprintf(FILE* stream, const char* fmt, ...)
{
	va_list va;
	va_start(va, fmt);
	int result = vfprintf(stream, fmt, va);
	va_end(va);
	return result;
}

int fseek(FILE* stream, off_t offset, int origin)
{
	if(stream && stream->seek)
		return stream->seek(stream, offset, origin);
	return -1;
}

size_t fread(void* addr, size_t bs, size_t num, FILE* stream)
{
	int i = 0, n = bs * num;
	int16_t tr = 0;
	for(int i = 0; i < n && (tr = fgetc(stream)) != EOF; ++i)
		((uint8_t*)(addr))[i] = tr;
	return i / bs;
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

/**
 * Formatting:
 * %[filler][space]
 * c: character
 * s: string
 * i: 10-base integer
 * d: 10-base integer
 */

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

void put_hex_32(uint32_t n)
{
	tty_putchar('0');
	tty_putchar('x');
	for(uint8_t i = 0; i < 8; ++i)
	{
		uint32_t j = (n >> ((8 - i - 1) * 4)) & 0xF;
		if(j < 10)
			tty_putchar('0' + j);
		else
			tty_putchar('A' - 10 + j);
	}
	tty_putchar(' ');
}

void put_hex_64(uint64_t n)
{
	tty_putchar('0');
	tty_putchar('x');
	for(uint8_t i = 0; i < 16; ++i)
	{
		uint32_t j = (n >> ((16 - i - 1) * 4)) & 0xF;
		if(j < 10)
			tty_putchar('0' + j);
		else
			tty_putchar('A' - 10 + j);
	}
	tty_putchar(' ');
}

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

	if(isnan(in))
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

	if(isinf(in))
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

/**
 * Formatting:
 * %[filler][space]
 * c: character
 * s: string
 * i: 10-base integer
 * d: 10-base integer
 */

int vsprintf(char *s, const char* fmt, va_list va)
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
			par.sign       = 0;
			par.plus       = 0;
			par.radix      = 10;
			par.uppercase  = 0;
			par.minlen     = 0;
			par.precision  = 0;
			par.minlen     = 0;
			par.maxlen     = INT32_MAX;
			par.align_left = 0;
			par.point      = '.';
			par.filler     = ' ';

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
					case '%': s[result++] = '%'; state = 2; break;
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
						s[result++] = (char)(va_arg(va, int));
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
									s[result++] = par.filler;
							s[result] = 0;
							//printf("%%s met for \"%s\", maxlen %i\n", str, par.maxlen);
							strncat(s, str, par.maxlen);
							result += min(len, par.maxlen);
							if(par.align_left)
								for(int k = 0; k < padsize; ++k)
									s[result++] = par.filler;
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
						int ol = ftoa_formated(s + result, r, &par);
						if(ol == -1)
							return -1;
						result += ol;
						state = 2;
						break;
					}
// change precision
					default:
					{
						if(fmt[j] >= '0' && fmt[j] <= '9')
						{
							ps = 1;
							if(flag_dot)
							{
								par.precision = par.precision * 10 + fmt[j] - '0';
							}
							else
							{
								par.minlen = par.minlen * 10 + fmt[j] - '0';
							}
						}
						else
						{
							s[result++] = fmt[j];
						}
					}
				}
			}

			if(state)
				i = j - 1;

// Integer value here
			if(state == 1)
			{
				par.filler = flag_zero ? '0' : ' ';
				int ol = itoa_formated(s + result, out, &par);
				if(ol == -1)
					return -1;
				result += ol;
			}
		}
		else
		{
			s[result++] = fmt[i];
		}
	}
	s[result] = 0;
	return result;
}

// RAM-only file descriptors manipulation
// May be used, however, for non-constant-size areas,
// even migrating areas if the needed data in streams
// is updated respectively

int16_t ram_getc(FILE* UNUSED(stream))
{
//	if(stream->position < stream->__size)
//		return *((uint8_t*)(stream->data)[++stream->position]);
	return EOF;
}

void fopen_from_memory(void* data, size_t UNUSED(size), const char* UNUSED(mode), FILE* f)
{
	f->data = data;
	//f->size = size;

}
