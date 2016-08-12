#ifndef _H_STRING
#define _H_STRING
#include"type.h"
static void inline memset(void *addr, u8 value, u32 size)
{
	while(size)
	{
			*((u8 *)(addr + size)) = value;
			size --;
	}
}

static void inline memcpy(void *dest, void *src, u32 size)
{
	u8 *d = (u8 *)dest;
	u8 *s = (u8 *)src;
	int i = 0;
	while(i< size)
	{
		*d = *s;
		d++;
		s++;
		i++;
	}
	/* asm */
}

static u32 inline str_cmp(char *a, char *b, u32 len)
{
	while( *a != '\0' && *b != '\0' && *a == *b && len > 0)	
	{
		len --;
		a++;
		b++;
	}
	return len > 1 ? -1 :0;
}	
#endif
