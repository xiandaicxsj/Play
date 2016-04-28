#include"math.h"
u32 round_up(u32 base , u32 align)
{
	u32 ret =  base;
	u32 r1;
	if (base & (align -1))
	{

		ret = base & (~(align - 1)) ;
		ret += align;
	}
	return ret;
}
	
