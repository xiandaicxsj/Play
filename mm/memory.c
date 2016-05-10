#include"page.h"
#include"math.h"

#define TEST
#define BUDDY_ALLOC

#ifdef TEST
	#include<stdio.h>
	#define MEM_KERN (1 << 0)
	#define MEM_USER (1 << 1)
#elif
	#include"memory.h"
	#include"debug.h"
	#include"kernel.lds.h"
#endif
/* now do not think of low memory */
#ifndef TEST
static u32 kernel_end;
static u32 low_mem_begin;
static u32 low_mem_end;
static u32 low_mem_used;
static u32 mem_size;
#endif

#ifdef BUDDY_ALLOC

#define MAX_ORDER 6 
struct free_area_t
{
	int order;
	void *map;
	u32 nr_free; /*  free pages */
	u32 pages_num;
	struct page *free_list;
};

struct pages_pool
{
	struct free_area_t free_area[MAX_ORDER];
};

struct page *pages_list;
struct pages_pool pgp;
#ifdef TEST
void list_add(struct page *pages, struct page *list)
{
	pages->next = list->next;
}

void list_del(struct page *pages, struct page *list)
{

}

void* kmalloc_low_mem(u32 size, u32 align, u32 flags)
{
		
	return NULL;
}
void* kmalloc(u32 size, u32 align, u32 flags)// virtual addr
{
	return NULL;
}

u32 _log(u32 num)
{
	return 0;
}
#endif
/* this is used for put the already_used low_memory
 * into buddy mempool
 */
#ifndef TEST
void trans_low_memory()
{
	u32 p_low_mem = virt_to_phy(low_mem_end);
	u32 nr_pages = p_low_mem & PAGE_MASK ? (p_low_mem >> PAGE_OFFSET) + 1: (p_low_mem >> PAGE_OFFSET);
	/* we use the alloc here,  mark this page to be used*/
	/* map is no needed here, because kmalloc_low_mem has mapped the memory*/
	_buddy_alloc_pages(nr_pages);
}
#endif

void init_buddy(u32 mem_size)
{
	u32 order = MAX_ORDER;
	u32 nr_pages;
	u32 pages_idx;
	struct free_area_t *area;

		/*  if we want to use 4K  */
	nr_pages = mem_size >> PAGE_OFFSET;
	area = &(pgp.free_area[order]);
	while (order > 0)
	{
		area->nr_free = 0;
		//pgp->free_area[order].pages_num = pages_num >> (order +1);
		area->free_list = NULL;
		area->map = kmalloc(nr_pages >> (order + 1), 64, MEM_KERN);
		order ++;
	}

	order = MAX_ORDER-1;
	pages_idx = 0;
	area = &(pgp.free_area[order]);
	area->nr_free = nr_pages >> order;
	while( pages_idx < (nr_pages >>order) )
	{
		set_bit(area->map, pages_idx);
		pages_idx ++;
	}
#ifndef TEST
	trans_low_memory();
#endif
	/* will be used when buddy is ok.
	 * free_low_memory();
	 */
}

static struct page *adjust_pages(struct page *pages, u32 lo_order, u32 hi_order,
				 struct free_area_t * area) 
{
	u32 size = 1 << hi_order;
	u32 pages_idx;
	while(lo_order < hi_order)
	{
		area --;
		hi_order --;
		size >>= 1;
		pages[size].order = hi_order;  
		pages_idx = ((u32) & pages[size]) >> hi_order;
		set_bit(area->map, pages_idx);
		area->nr_free ++;
	}
	return pages;
}

struct pages *find_buddy_pages(u32 pfn, u32 order)
{
	u32 pages_idx = pfn ^ (1 << order);
	return pages_list + pages_idx;
}

/* 
 * the order or the size of certain pages is store in 
 * the first page of the pagses.
 */
void _buddy_free_page(struct page *pages)
{
	u32 pages_idx;
	struct  free_area_t *area;
	struct page *buddy_pages;
	u32 pfn = pages->pfn;
	u32 order = pages->order;

	area = &(pgp.free_area[order]);

	/* just write this, not sure of the count use */
	pages->count--;
	if (!pages->count)
		return ;

	while(order < MAX_ORDER)
	{
		pages_idx = pfn >> (order + 1);
		/* 
		 * if no buddy is found, juest put the page in the free_list,
		 * we do not del with the count of struct page
		 */
		if (!test_bit(area->map, pages_idx))
		{
			set_bit(area->map, pages_idx);
			area->nr_free ++;
			/* we need to set pages.. count -> 0 ,same with alloc */
			list_add(pages, area->free_list);
			break;
		}

		/* list del the buddy pages */
		buddy_pages = find_buddy_pages(pfn, order);
		/* not sure if find_buddy return NULL */
		if (!buddy_pages)
			return ;

		buddy_pages->order = -1;
		list_del(buddy_pages, area->free_list);
		clear_bit(area->map, pages_idx);
		area->nr_free--;

		/* update pages to be the head of the buddy, merge the pages into high order list*/
		pages = (pfn >> order) & 0x1 ? buddy_pages : pages;
		area ++;
		order ++;
		pfn = pages->pfn;
	}
}

struct page * _buddy_alloc_pages(u32 num)
{
	/* need add _log function */
	u32 order = _log(num);
	u32 cur_order = order;
	struct page *tmp;
	struct  free_area_t *area;
	while(cur_order < MAX_ORDER)
	{
		area = &(pgp.free_area[cur_order]);
		if (!area->nr_free) {
			cur_order++;
			continue;
		}
		if (!area->free_list)
			return NULL;
		tmp = area->free_list;
		list_del(tmp, area->free_list);
		return adjust_pages(tmp, order, cur_order, area);
   }
	return NULL;
}

#ifdef TEST
int main()
{
	init_buddy(256*1024*1024);
	return 0;
}
#endif

u32 init_page_struct(struct page *cur, struct page* pre, u32 pfn)
{
	if (pre)
		pre->next = cur;
	cur->next = pre;
	cur->pfn = pfn;
	cur->order = -1;
	cur->count = 0;
}

u32 link_pages(struct page *head, u32 nr_pages)
{
	u32 pfn = 0;
	struct page * prev = head;
	struct page * next = head;
	init_page_struct(next, NULL, pfn);
	while(nr_pages)
	{
		next = prev ++; /* may be wrong */
		pfn ++;
		init_page_struct(next, prev, pfn);
		nr_pages --;
	}
}

u32 init_pages_list(u32 mem_size)
{
	u32 nr_pages = mem_size >> PAGE_OFFSET;
	u32 struct_size = nr_pages * sizeof(struct page);
	u32 nr_pages_s = struct_size & PAGE_MASK ? (struct_size >> PAGE_OFFSET) + 1: (struct_size >> PAGE_OFFSET);
	pages_list = (struct page *)kmalloc_low_mem(nr_pages_s << PAGE_OFFSET, 0, MEM_KERN);
	link_pages(pages_list, mem_size >> PAGE_OFFSET);
}

#endif
#ifndef TEST
static void* kmalloc_low_mem(u32 size, u32 align, u32 flags)
{
	u32 addr;
	if ( size > mem_size )
		return NULL;
	if ( align )
		low_mem_end = round_up(low_mem_end, align);
	addr = low_mem_end;
	low_mem_end += size;
	if (flags && MEM_KERN) {
		/* map kernel page is needed */
	}
	return (void *)addr;
}
static void setup_low_memory()
{
	low_mem_begin = (u32)_bss_end;
	low_mem_end = low_mem_begin;
	low_mem_used = 1;
	return ;
}

static void * _kmalloc(u32 size, u32 align, u32 flags)
{
#ifdef BUDDY_ALLOC 
		struct page * pages = _buddy_alloc(size, align);
		if (flags && MEM_KERN) {
			/* we need to map the pages, if it is used by kernel
			 * do not need to map the pages used for user space
			 */
		}
#endif
	return NULL;
}

void* kmalloc(u32 size, u32 align, u32 flags)// virtual addr
{
	if ( low_mem_used )
		return kmalloc_low_mem(size, align, flags);
	return _kmalloc(size, align, flags);
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
	init_pages_list(mem_size);
	init_buddy(mem_size);
#endif
	return ;
}
#endif
