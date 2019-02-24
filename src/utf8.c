#include "utf8.h"

#include "string.h"
#include "graphics/surface.h"
#include "memory.h"

size_t utf8_convert(const char* text, uint32_t* out)
{
	size_t i, j, n = strlen(text) + 1, k, m = 0;
	uint32_t buf;
	uint8_t mask1 = 0;
	for(i = 0; text[i]; ++i)
	{
		buf = 0;
		mask1 = 0b01111111;
		k = 0;
		if((text[i] & 0b11000000) == 0b11000000)
		{
			for(k = 1; ((text[i] << k) & 0b10000000) && k <= 6; ++k)
			{
				mask1 >>= 1;
			}
		}
		if(k)
			--k;
		buf = (((uint32_t)(text[i])) & mask1) << (k * 6);
		j = i + k;
		for(; i < j && i < n - 1; ++i)
		{
			buf |= (((uint32_t)(text[i + 1]) & 0b111111) << ((j - i - 1) * 6));
		}
		out[m++] = buf;
	}
	out[m] = 0;
	return m;
}

size_t utf8_strlen(const uint32_t *str)
{
	size_t result = 0;
	while(str[result] != 0)
	{
		result++;
	}
	return result;
}
