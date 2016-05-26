#ifndef _H_STRING
#define _H_STRING
#include"type.h"
void inline memset(void *addr, u8 value, u32 size)
{
	while(size)
	{
			*((u8 *)(addr + size)) = value;
			size --;
	}
}
void inline memcpy(void *dsr, void *addr, u32 size)
{

}

u32 str_cmp(char *a, char *b, u32 len)
{
	while( *a != '\0' && *b != '\0' && *a != *b && len > 0)	
	{
		len --;
		a++;
		b++;
	}
	return len > 0 ? -1 :0;
}	
#endif
