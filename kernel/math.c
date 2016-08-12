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
u32 inline _log(u32 num)	
{
	u32 count = 0;
	int tmp =1;
	while(1)
	{
		if(num <= tmp)
			break;
		tmp = tmp << 1;
		count ++;
	}
        return count;
}
