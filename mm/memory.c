#include"memory.h"
#include"page.h"
#include"math.h"
#include"debug.h"
#include"kernel.lds.h"
/* now do not think of low memory */
static u32 kernel_end;
static u32 low_mem_begin;
static u32 low_mem_end;
static u32 low_mem_used;
static u32 mem_size;

#ifdef BUDDY_ALLOC

#define MAX_ORDER 6
struct free_area
{
	int order;
	void *map;
	u32 nr_free; /*  free pages */
	u32 pages_num;
	struct page *free_list;
};

struct pages_pool
{
	struct free_area[MAX_ORDER];
};

struct pages_pool pgp;
void init_buddy(u32 mem_size)
{
		u32 order = MAX_ORDER;
		u32 pages_num;
		int pages_index;

		/*  if we want to use 4K  */
		pages_num = mem_size >> PAGE_OFFSET;
		while (order >> 0)
		{
			pgp->free_area[order].nr_free = 0;
			//pgp->free_area[order].pages_num = pages_num >> (order +1);
			pgp->free_area[order].free_list = NULL;
			pgp->free_area[order].map = kmalloc(pages_num >> (order + 1), 64);
			order ++;
		}
		order = MAX_ORDER-1;
		pages_index = 0;
		pgp->free_area[order].nr_free = pages_num >> order;
		while( pages_index < (pages_num >>order) )
		{
			set_bit(pgp->map, pages_index);
			pages_index ++;
		}
	/* will be used when buddy is ok.
	 * free_low_memory();
	 */
}

struct page* _buddy_alloc(int size, u32 align)
{

}

static struct pgae *adjust_pages(struct page *pages, u32 lo, u32 hi,
				 struct free_area *area) 
{
	u32 size = 1 << hi;
	while(low < hi)
	{
		area --;
		hi --;
		size >>= 1;
		pages[size].order = hi;  
		index = ((u32) &page[size]) >> hi;
		set_bit(area->map, index);
		area->nr_free ++;
	}
	return page;
}

void _buddy_free_page(struct *pages)
{
	u32 order = pages->order;
	while()
}

struct page * _buddy_alloc_page(u32 num)
{
	u32 order = _log(num);
	u32 cur_order = order;
	struct page *tmp;
	struct  free_area *area;
	while(cur_order < MAX_ORDER)
	{
		if (!pgd->free_area[cur_order].nr_free) {
				cur_order++;
				continue
		}
		area = &(pdg->free_area[cur_order])
		if (area->free_list)
				BUG(); 
		tmp = area->free_list;
		list_del(tmp, area->free_list);
		return adjust_pages(tmp, order, cur_order, area);
  }
}

u32 _buddy_free_page(struct page * page, u32 order)
{

}

#endif
static void* kmalloc_low_mem(u32 size, u32 align)
{
	u32 addr;
	if ( size > mem_size )
		return NULL;
	if ( align )
		low_mem_end = round_up(low_mem_end, align);
	addr = low_mem_end;
	low_mem_end += size;
	return (void *)addr;
}
static void setup_low_memory()
{
	low_mem_begin = (u32)_bss_end;
	low_mem_end = low_mem_begin;
	low_mem_used = 1;
	return ;
}

static  void * _kmalloc(u32 size, u32 align)
{
#ifdef BUDDY_ALLOC 
		return _buddy_alloc(size, align);
#endif
	return NULL;
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
#ifdef BUDDY_ALLOC
	//init_buddy();
#endif
	return ;
}
