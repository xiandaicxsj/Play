#include"math.h"
u32 round_up(u32 base , u32 align)
{
	if (base & (align -1))
		return base & (~(align - 1)) + align;
	return base;
}
	
