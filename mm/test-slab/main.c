#include"slab.h"
#include<stdio.h>
int main()
{
	void *ptr;
	init_mem_cache();
	ptr = kmalloc_from_normal_slab(32, 0);
	printf("ptr addr %llx\n", ptr);

}
