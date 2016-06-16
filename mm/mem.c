#include"math.h"
#include"bitop.h"

#define BUDDY_ALLOC

#ifdef TEST_MEM
	#include<stdio.h>
	#include<stdlib.h>
	#define MEM_KERN (1 << 0)
	#define MEM_USER (1 << 1)
#else
	#include"mem.h"
	#include"debug.h"
	#include"kernel.lds.h"
#endif
/* now do not think of low memory */
#ifndef TEST_MEM
static u32 kernel_end;
static u32 low_mem_begin;
static u32 low_mem_end;
static u32 low_mem_alloc_used;
static u32 mem_size;
#endif

#ifdef BUDDY_ALLOC

#define MAX_ORDER 10
struct free_area_t
{
	int order;
	void *map;
	u32 nr_free; /*  free pages */
	u32 pages_num;
	struct page *free_pages;
};

struct pages_pool
{
	struct free_area_t free_area[MAX_ORDER];
};

struct page *pages_list;
struct pages_pool pgp;
#ifdef TEST_MEM
void* kmalloc_low_mem(u32 size, u32 align)
{
	return malloc(size);
}
void* kmalloc(u32 size, u32 align, u32 flags)// virtual addr
{
	return malloc(size);
}

#endif
/* this is used for put the already_used low_memory
 * into buddy mempool
 */
static void merge_low_memory();

struct page *pfn_to_page(u32 pfn)
{
	return pages_list + pfn;
}

void init_buddy(u32 mem_size)
{
	u32 order;
	u32 nr_pages;
	u32 pages_idx;
	u32 pfn;
	struct free_area_t *area;
	struct page *pages;

	/*  if we want to use 4K  */
	order = 0;
	nr_pages = mem_size >> PAGE_OFFSET;
	while (order < MAX_ORDER )
	{
		area = &(pgp.free_area[order]);
		area->nr_free = 0;
		area->order = order;
		//pgp->free_area[order].pages_num = pages_num >> (order +1);
		area->free_pages = NULL;
		area->map = kmalloc((nr_pages >> (order + 1))/sizeof(char) + 1, 0, MEM_KERN);
		order ++;
	}

	/* we need to del with order = MAX_ORDER -1 specially  ?? */
	/* the map of MAX_ORDER -1 is not used */

	order = MAX_ORDER-1;
	pfn = 0;
	pages_idx = 0;
	area = &(pgp.free_area[order]);
	area->nr_free = nr_pages >> order;
	pages_idx = pfn >> order;
	while( pages_idx < (nr_pages >>order) )
	{
		pages = pfn_to_page(pfn);
		pages->order = order;

		if (!area->free_pages)
			area->free_pages = pages;
		else
			list_add(&(pages->list), &(area->free_pages->list));

		pages_idx ++;
		pfn += 1 << order;
	}
#ifndef TEST_MEM
	merge_low_memory();
	low_mem_alloc_used = 0;
#endif
	/* will be used when buddy is ok.
	 * free_low_memory();
	 */
}

static struct page *adjust_pages(struct page *pages, u32 lo_order, u32 hi_order,
				 struct free_area_t *area) 
{
	u32 size = 1 << hi_order;
	struct page *page_head;
	u32 pfn = pages->pfn;
	u32 pages_idx = pfn >> (hi_order + 1);

	/* for low == hi , only update order of the pages*/
	if (lo_order == hi_order)
		goto out;
	while(lo_order < hi_order)
	{
		area --;
		hi_order --;
		size >>= 1;

		/* update pages info */
		page_head = pfn_to_page(pfn + size);
		page_head->order = hi_order;  

		/* do we need to list_free */
		/* update area */
		pages_idx = page_head->pfn >> (hi_order + 1);
		/* why set_bit here, indicate his buddy is used*/
		set_bit(area->map, pages_idx);
		if(!area->free_pages) {
			list_init(&pages->list);
			area->free_pages = page_head;
		}
		else
			list_add(&(page_head->list), &(area->free_pages->list));
		area->nr_free ++;
	}
	/* very important */
out:
	pages->order = lo_order;
	return pages;
}

struct page *find_buddy_pages(u32 pfn, u32 order)
{
	return pfn_to_page(pfn ^ (1 << order));
}

/* 
 * the order or the size of certain pages is store in 
 * the first page of the pagses.
 */
static void _buddy_free_pages(struct page *pages)
{
	u32 pages_idx;
	struct  free_area_t *area;
	struct page *buddy_pages;
	u32 pfn = pages->pfn;
	u32 order = pages->order;

	/* just write this, not sure of the count use */
	/*
	pages->count--;
	if (!pages->count)
		return ;
	*/

	while(order < MAX_ORDER)
	{
		area = &(pgp.free_area[order]);
		pages_idx = pfn >> (order + 1);
		/* 
		 * if no buddy is found, just put the page in the free_pages,
		 * we do not del with the count of struct page
		 */
		if (!test_bit(area->map, pages_idx))
		{
			set_bit(area->map, pages_idx);
			area->nr_free ++;
			/* we need to set pages.. count -> 0 ,same with alloc */
			pages->order = order;

			if (!area->free_pages)
			{
				list_init(&pages->list);
				area->free_pages = pages;
			}
			else
			    list_add(&(pages->list), &(area->free_pages->list));
			break;
		}

		/* list del the buddy pages */
		buddy_pages = find_buddy_pages(pfn, order);
		/* not sure if find_buddy return NULL */
		if (!buddy_pages)
			return ;

		buddy_pages->order = -1;
		area->nr_free--;
		if (!area->nr_free) {
			area->free_pages = NULL;
		} else {
			if (area->free_pages == buddy_pages)
				area->free_pages =
					container_of(buddy_pages->list.next, struct page, list);
			list_del(&(buddy_pages->list));
		}
		/* we need to merge the 2 buddy pages into high pages list */
		u32 map = *((u32 *)area->map);
		clear_bit(area->map, pages_idx);
		map = *((u32 *)area->map);

		/* update pages to be the head of the buddy, merge the pages into high order list*/
		pages = (pfn >> order) & 0x1 ? buddy_pages : pages;
		area ++;
		order ++;
		pfn = pages->pfn;
	}
}

static struct page * _buddy_alloc_pages(u32 num)
{
	/* need add _log function */
	u32 order = _log(num);
	u32 cur_order = order;
	struct page *tmp;
	struct free_area_t *area;
	while(cur_order < MAX_ORDER)
	{
		area = &(pgp.free_area[cur_order]);
		if (cur_order == MAX_ORDER -1 &&
		    (!area->nr_free ))
			return NULL; /* it is means no too huge pages, we may
			need to collect other pages*/
		if (!area->nr_free) {
			cur_order++;
			continue;
		}

		if (!area->free_pages)
			return NULL;
		tmp = area->free_pages; 

		area->nr_free --;
		if (!area->nr_free)
			area->free_pages = NULL;
		else
			area->free_pages = container_of(tmp->list.next, struct page, list);
		list_del(&tmp->list);
		/* map is not used for order = MAX_ORDER -1 */
		if (cur_order != MAX_ORDER -1)
			clear_bit(area->map, tmp->pfn >> (cur_order + 1));
		/*
		 * if we just del the tmp list, then free_pages will poit to 
		 * an useless place
		 */
		return adjust_pages(tmp, order, cur_order, area);
 	}
	return NULL;
}

static u32 init_page_struct(struct page *cur, u32 pfn)
{
	cur->pfn = pfn;
	cur->order = -1;
	cur->count = 0;
	list_init(&cur->list);
}

static u32 link_pages(struct page *head, u32 nr_pages)
{
	u32 pfn = 0;
	struct page * cur = head;
	while(nr_pages)
	{
		init_page_struct(cur, pfn);
		nr_pages --;
		cur ++;
		pfn ++;
	}
}

static u32 init_pages_list(u32 mem_size)
{
	u32 nr_pages = mem_size >> PAGE_OFFSET;
	u32 struct_size = nr_pages * sizeof(struct page);
	u32 nr_pages_s = struct_size & PAGE_MASK ? (struct_size >> PAGE_OFFSET) + 1: (struct_size >> PAGE_OFFSET);
	pages_list = (struct page *)kmalloc(nr_pages_s << PAGE_OFFSET, 0, MEM_KERN);
	/* no need to link page here */
	link_pages(pages_list, mem_size >> PAGE_OFFSET);
}

#ifndef TEST_MEM
static void merge_low_memory()
{
	u32 p_low_mem = virt_to_phy(low_mem_end);
	u32 nr_pages = p_low_mem & PAGE_MASK ? (p_low_mem >> PAGE_OFFSET) + 1: (p_low_mem >> PAGE_OFFSET);
	/* we use the alloc here,  mark this page to be used*/
	/* map is no needed here, because kmalloc_low_mem has mapped the memory*/
	while(nr_pages)
	{
		_buddy_alloc_pages(1);
		nr_pages --;
	}
}

#endif

#ifdef TEST_MEM
void show_page(struct page *pages)
{
	printf("   pages - order %d, pfn %d\n", pages->order, pages->pfn);
}

void show_pages(struct list_head *list)
{
	struct list_head *head = list;
	struct list_head *cur;
	show_page(container_of(head, struct page, list));
	
	cur = head->next;
	while(cur != head)
	{
		show_page(container_of(cur, struct page, list));
		cur = cur->next;
	}
}

void show_buddy_area(int order)
{
	struct free_area_t *area = &pgp.free_area[order];
	if (order < 0)
		return ;
	printf("area - order: %d size %x\n", order, 1u << order);
	printf("area - free pages %d\n", area->nr_free);
	printf("area - map %x\n", *((u32 *)area->map));
	if (area->nr_free)
		show_pages(&(area->free_pages->list));
	printf("\n\n");
}

void show_buddy_info()
{
	int order = MAX_ORDER -1;
	while(order >= 0)
	{
		show_buddy_area(order);
		order --;
	}
}

int main()
{
	struct page *pages;
	struct page *pages0;
	struct page *pages2;
	struct page *pages3;
	struct page *pages31;
	u32 mem_size = 256 *1024*1024;
	init_pages_list(mem_size);
	init_buddy(mem_size);
	/*
	pages0 = _buddy_alloc_pages(1);
	pages2 = _buddy_alloc_pages(2);
	pages3 = _buddy_alloc_pages(3);
	show_buddy_info();
	pages31 = _buddy_alloc_pages(3);
	show_buddy_info();
	printf("free page0\n");
	_buddy_free_pages(pages0);
	show_buddy_info();
	printf("free page2\n");
	_buddy_free_pages(pages2);
	show_buddy_info();
	printf("free page3\n");
	_buddy_free_pages(pages3);
	show_buddy_info();
	printf("free page31\n");
	_buddy_free_pages(pages31);
	show_buddy_info();
	//show_buddy_info();
	*/
	pages = _buddy_alloc_pages(1);
	show_buddy_info();
	pages = _buddy_alloc_pages(1);
	show_buddy_info();
	pages = _buddy_alloc_pages(1);
	show_buddy_info();
	pages = _buddy_alloc_pages(1);
	show_buddy_info();
	pages = _buddy_alloc_pages(1);
	show_buddy_info();
	return 0;
}
#endif
#endif
#ifndef TEST_MEM
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
static struct page * _buddy_alloc(u32 size, u32 align)
{
	/* align is useless */
	u32 nr_pages = size >> PAGE_OFFSET;
	nr_pages += (size & PAGE_MASK ? 1 : 0);
	return _buddy_alloc_pages(nr_pages);
}

static void setup_low_memory()
{
	low_mem_begin = (u32)_bss_end;
	low_mem_end = low_mem_begin;
	low_mem_alloc_used = 1;
	return ;
}

/* return kernel virtual */
static void * _kmalloc(u32 size, u32 align)
{
#ifdef BUDDY_ALLOC 
		struct page * pages = _buddy_alloc(size, align);
#endif
	return phy_to_virt(pages->pfn << PAGE_OFFSET);
}

/* interface */
struct page *kalloc_page(u32 flags)
{
	kalloc_pages(1, flags);
}

struct page *kalloc_pages(u32 nr, u32 flags)
{
	struct page *pages;
	if (low_mem_alloc_used)
		pages = kmalloc_low_mem(nr * PAGE_SIZE, PAGE_SIZE);
	else
		pages = _buddy_alloc_pages(nr);
	/*
	if (flags & MEM_KERN)
		return NULL; */
		/* bugs */
	return pages;
}

u32 kfree(void *addr)
{

}

u32 kfree_page(struct page* page)
{
	_buddy_free_pages(page);
}

void* kmalloc(u32 size, u32 align, u32 flags)// virtual addr
{
	void * addr ;
	if ( low_mem_alloc_used )
		addr = kmalloc_low_mem(size, align);
	else
		addr = _kmalloc(size, align);
	/* need map */
	if (flags & MEM_KERN)

	return addr;
		/* bugs */
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
