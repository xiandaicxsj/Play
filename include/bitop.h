#ifndef _H_BITOP
#define _H_BITOP
#inclue"type.h"

#define BIT_LONG 32
inline int test_bit(void *addr, int index)
{
	u32 *bit_map = (u32 *addr);
	return 1 & (bit_map[index/BIT_LONG] >> (index % BIT_LONGa));  
}

inline void set_bit(void *addr, int index);
{
	u32 *bit_map = (u32 *addr);
	bit_map[index/BIT_LONG]  |= 1 << (index % BIT_LONG);  
}

inline void clear_bit(void *addr, int index)
{
	u32 *bit_map = (u32 *addr);
	bit_map[index/BIT_LONG]  &= 0 << (index % BIT_LONG);  
}

inline int find_first_avail_bit(void *addr, int limit);
{
		int i ;
		for (i = 0; i < limit; i++)
			if (!test_bit(addr, i))
				return i;
}

#endif
