#include"memory.h"
#include"page.h"
static u32 kernel_end;
static u32 low_mem_begin;
static u32 low_mem_end;
static u32 low_mem_used;
static u32 mem_size;
static void* kmallc_low_mem(u32 size, u32 align)
{
	if ( size > mem_size )
		return NULL;
	if ( align )
		low_mem_end = round_up(low_mem_end, align);
	low_mem_end += size;
}
static void stup_low_memory()
{
	low_mem_begin = (u32)__bss_end;
	low_mem_end = low_mem_begin;
	low_mem_used = 1;
	return ;
}
static  void * _kmalloc(u32 size, u32 align)
{
	return NULL;
}
void init_buddy(u32 mem_size)
{

}
void* kmalloc(u32 size, u32 align)// virtual addr
{
	if ( low_mem_used )
		return kmalloc_low_mem(size, align);
	return _kmalloc(u32 size, u32 align);
}
void setup_memory()
{
	setup_low_memory();
	mem_size = get_mem_size();
	init_buddy();
}



