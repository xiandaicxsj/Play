#ifndef _H_BITOP
#define _H_BITOP
#include"type.h"

inline int test_bit(void *addr, int index);
inline void set_bit(void *addr, int index);
inline void clear_bit(void *addr, int index);
inline int find_first_avail_bit(void *addr, int limit);
#endif
