#ifndef FONT_H
#define FONT_H

#include <stdint.h>
#include <stddef.h>

size_t utf8_convert(const char*, uint32_t*);
size_t utf8_strlen (const uint32_t *str);

#endif // FONT_H
