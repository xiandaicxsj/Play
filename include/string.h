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
#endif
