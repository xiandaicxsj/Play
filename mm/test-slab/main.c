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
	ptr = kmalloc_from_normal_slab(512, 0);
	ptr = kmalloc_from_normal_slab(512, 0);
	ptr = kmalloc_from_normal_slab(512, 0);
	ptr = kmalloc_from_normal_slab(512, 0);
	ptr = kmalloc_from_normal_slab(512, 0);
	ptr = kmalloc_from_normal_slab(512, 0);
	ptr = kmalloc_from_normal_slab(512, 0);
	ptr = kmalloc_from_normal_slab(2048, 0);
	ptr = kmalloc_from_normal_slab(2048, 0);
	ptr = kmalloc_from_normal_slab(2048, 0);
	ptr = kmalloc_from_normal_slab(2048, 0);
	ptr = kmalloc_from_normal_slab(2048, 0);
	ptr = kmalloc_from_normal_slab(2048, 0);
	ptr = kmalloc_from_normal_slab(2048, 0);
	dump_smc_all();
	free_from_mem_cache(ptr);
	dump_smc_all();
	//free_from_mem_cache(ptr);
	//dump_smc_all();
}
