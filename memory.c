#include"memory.h"
#include"page.h"
#include"math.h"
#include"kernel.lds.h"
/* now do not think of low memory */
static u32 kernel_end;
static u32 low_mem_begin;
static u32 low_mem_end;
static u32 low_mem_used;
static u32 mem_size;
#define MAX_ORDER 6
struct page_pool
{
		void *mem_map;
		struct page * free_pages[MAX_ORDER];
};
static void* kmalloc_low_mem(u32 size, u32 align)
{
	void * addr;
	if ( size > mem_size )
		return NULL;
	asm volatile ("movl %%eax, %%edx"::"a"(low_mem_end):); 
	while(1);
	if ( align )
		low_mem_end = round_up(low_mem_end, align);
	addr = (void *)low_mem_end;
	low_mem_end += size;
	return phy_to_virt(addr);
}
static void setup_low_memory()
{
	low_mem_begin = (u32)_bss_end;
	asm volatile ("movl %%eax, %%edx"::"a"(_bss_end):); 
	low_mem_end = low_mem_begin;
	low_mem_used = 1;
	return ;
}
static  void * _kmalloc(u32 size, u32 align)
{
	return NULL;
}
void init_buddy()
{
		return ;
}
void* kmalloc(u32 size, u32 align)// virtual addr
{
	if ( low_mem_used )
		return kmalloc_low_mem(size, align);
	return _kmalloc(size, align);
}

u32 static get_mem_size()
{
	return 256*1024*1024;
}
/* now the memory is not set properily */
void setup_memory()
{
	setup_low_memory();
	mem_size = get_mem_size();
	//init_buddy();
	return ;
}
