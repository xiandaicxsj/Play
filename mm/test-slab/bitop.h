#ifndef _H_BITOP
#define _H_BITOP
#include"type.h"

int test_bit(void *addr, int index);
void set_bit(void *addr, int index);
void clear_bit(void *addr, int index);
int find_first_avail_bit(void *addr, int limit);
#endif
