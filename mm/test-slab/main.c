#include"slab.h"
#include<stdio.h>
int main()
{
	void *ptr;
	init_kmem_cache();
	ptr = kmalloc_from_normal_slab(32, 0);
	ptr = kmalloc_from_normal_slab(64, 0);
	ptr = kmalloc_from_normal_slab(32, 0);
	ptr = kmalloc_from_normal_slab(32, 0);
	printf("ptr addr %llx\n", ptr);
	free_from_mem_cache(ptr);
}
