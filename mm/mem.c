#include"math.h"
#include"bitop.h"
#include"test.h"
#include"mem.h"

#define BUDDY_ALLOC

#ifdef TEST_MEM
	#include<stdio.h>
	#include<stdlib.h>
#else
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

#ifdef BUDDY_ALLOC

#define MAX_ORDER 10
struct free_area_t
{
	int order;
	u32 nr_free; /*  free pages */
	u32 pages_num;
	struct page *free_pages;
};

struct pages_pool
{
	struct free_area_t free_area[MAX_ORDER + 1];
};

struct page *pages_list;
struct pages_pool pgp;

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
	struct free_area_t *area;
	struct page *pages;

	/*  if we want to use 4K  */
	order = 0;
	nr_pages = mem_size >> PAGE_SHIFT;

	/* we need to set the order files of each page to be -1 */
	zero_all_pages_order(nr_pages);

	while (order <= MAX_ORDER) {
		area = &(pgp.free_area[order]);
		area->nr_free = 0;
		area->order = order;
		/* or we may init area->free_list here */
		//pgp->free_area[order].pages_num = pages_num >> (order +1);
		area->free_pages = NULL;
		order ++;
	}

	/* we need to del with order = MAX_ORDER -1 specially  ?? */
	/* the map of MAX_ORDER -1 is not used */

	order = MAX_ORDER;
	area = &(pgp.free_area[order]);

	area->nr_free = nr_pages >> order;

	pages_idx = 0;
	size = 1  << order;

	while(pages_idx < nr_pages) {

		pages = pfn_to_page(pages_idx);
		pages->order = order;
		add_area_pages(area, pages); 

		pages_idx += size;
	}

	merge_low_memory();
	low_mem_alloc_used = 0;
	/* will be used when buddy is ok.
	 * free_low_memory();
	 */
}

static u32 get_buddy_idx(u32 pidx, u32 order)
{
	return pidx ^ (1 << order);
}	

static struct page *get_buddy_page(struct page *page, u32 order)
{
	u32 buddy_idx = get_buddy_idx(page->pfn, order);
	return pfn_to_page(buddy_idx);
}	

static struct page *adjust_pages(struct page *pages, u32 lo_order, u32 hi_order,
				 struct free_area_t *area) 
{
	struct page *tmp_pages;
	u32 page_idx;
	u32 size;

	size = 1 << hi_order;
	page_idx = pages->pfn;
	/* for low == hi , only update order of the pages*/
	if (lo_order == hi_order)
		goto out;

	while(lo_order < hi_order) {
		area --;
		hi_order --;
		size >> = 1;

		/* get the next page */
		tmp_pages = pages[size];
		tmp_pages->order = hi_order;

		add_area_page(area, tmp_pages);
	}
	/* very important */
out:
	pages->order = lo_order;
	return pages;
}

/* 
 * the order or the size of certain pages is store in 
 * the first page of the pagses.
 */
static u32 check_buddy(struct page *pages, struct page *buddy, u32 order)
{
	return buddy->order == order ? 1: 0;
}

static void add_area_pages(struct free_area_t *area, struct page *pages)
{
	if(!area->free_pages) {
		list_init(&pages->list);
		area->free_pages = page;
	} else
		list_add(&(page->list), &(area->free_pages->list));
	area->nr_free ++;
}

static u32 del_area_pages(struct free_area_t *area, struct page *pages)
{
	ASSERT(!area->nr_free);
	area->nr_free--;
	if (!area->nr_free)
		area->free_pages = NULL;
	else if (area->free_pages == buddy_pages)
		area->free_pages = container_of(pages->list.next, struct page, list);
	list_del(&(pages->list));
}

static zero_page_order(struct page *pages)
{
	pages->order = -1;
}

static void _buddy_free_pages(struct page *pages)
{
	struct  free_area_t *area;
	struct page *buddy_page;
	u32 order = pages->order;
	u32 page_idx;
	u32 buddy_idx;
	u32 c_idx;

	/* just write this, not sure of the count use */
	/*
	pages->count--;
	if (!pages->count)
		return ;
	*/

	while(order < MAX_ORDER) {
		area = &(pgp.free_area[order]);
		page_idx = pages->pfn;

		buddy_idx = get_buddy_idx(page_idx, order);
		buddy_page = pfn_to_page(buddy_idx);

		if (!check_buddy(buddy_page, pages,order)) {
			pages->order = order;
			add_area_page(area, pages);
			break;
		}

		zero_page_order(pages);	

		zero_page_order(buddy_page);	
		del_area_pages(area, buddy_pages);

		c_idx = page_idx & buddy_idx;
		pages = pfn_to_page(c_idx);

		area ++;
		order ++;
	}
}

static struct page * _buddy_alloc_pages(u32 num)
{
	/* need add _log function */
	u32 lo_order = _log(num);
	u32 hi_order = order;
	struct page *pages;
	struct free_area_t *area;
	while(hi_order < MAX_ORDER) {
		area = &(pgp.free_area[hi_order]);
		if (hi_order == MAX_ORDER -1 && (!area->nr_free))
			return NULL; /* it is means no too huge pages, we may
			need to collect other pages*/
		if (!area->nr_free) {
			hi_order++;
			continue;
		}

		/* should replace this with ASSERT(area->free_pages) */
		if (!area->free_pages)
			return NULL;
		pages = area->free_pages; 
		del_area_page(area, tmp);

		return adjust_pages(pages, lo_order, hi_order, area);
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
	u32 nr_pages = mem_size >> PAGE_SHIFT;
	u32 struct_size = nr_pages * sizeof(struct page);
	u32 nr_pages_s = struct_size & PAGE_MASK ? (struct_size >> PAGE_SHIFT) + 1: (struct_size >> PAGE_SHIFT);
	pages_list = (struct page *)kmalloc(nr_pages_s << PAGE_SHIFT, 0, MEM_KERN);
	/* no need to link page here */
	link_pages(pages_list, mem_size >> PAGE_SHIFT);
}

static void merge_low_memory()
{
	u32 p_low_mem = virt_to_phy(low_mem_end);
	u32 nr_pages = p_low_mem & PAGE_MASK ? (p_low_mem >> PAGE_SHIFT) + 1: (p_low_mem >> PAGE_SHIFT);
	/* we use the alloc here,  mark this page to be used*/
	/* map is no needed here, because kmalloc_low_mem has mapped the memory*/
	while(nr_pages) {
		_buddy_alloc_pages(1);
		nr_pages --;
	}
}

#endif

/*
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

*/
int main()
{
	struct page *pages;
	struct page *pages0;
	struct page *pages2;
	struct page *pages3;
	struct page *pages31;
	u32 mem_size = 256 *1024*1024;
	init_pages_list(mem_size);
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
	u32 nr_pages = size >> PAGE_SHIFT;
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
	return (void *)phy_to_virt(pages->pfn << PAGE_SHIFT);
}

/* interface */
struct page *kalloc_page(u32 flags)
{
	struct page *pg;

	pg = kalloc_pages(1, flags);
	/* map page */

#ifndef TEST_MEM
	if (flags & MEM_KERN)
		map_page(addr_to_pfn(phy_to_virt(pfn_to_addr(pg->pfn))), pg->pfn, flags, NULL);
#endif
	return pg;
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
	u32 aj_addr;
	u32 aj_size;

	if ( low_mem_alloc_used )
		addr = kmalloc_low_mem(size, align);
	else
		addr = _kmalloc(size, align);

#ifndef TEST_MEM
	aj_addr = round_down((u32) addr, PAGE_SIZE);
	aj_size = round_up((u32)addr + size, PAGE_SIZE) - aj_addr;

	if (flags & MEM_KERN) {

		while(aj_size > 0) {
			map_page(addr_to_pfn(aj_addr), virt_to_phy(addr_to_pfn(aj_addr)), flags, NULL);
			aj_addr += PAGE_SIZE;
			aj_size -= PAGE_SIZE;
		}
	}
#endif

	return addr;
		/* bugs */
}

u32 static get_mem_size()
{
	/* here we juest write */
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


#ifdef TEST_MEM
void* kmalloc(u32 size, u32 align, u32 flags)// virtual addr
{
	return malloc(size);
}

u32 kfree(void *t)
{
	free(t);
	return 0;
}
/* not sure of this */
struct page *kalloc_page(u32 flags)
{
	struct page *tmp = (struct page *)malloc(sizeof(*tmp));
	tmp->pfn = (u32) malloc(PAGE_SIZE); 
	return tmp;
}

struct page *kalloc_pages(u32 nr, u32 flags)
{
	struct page *tmp = (struct page *)malloc(sizeof(*tmp));
	tmp->pfn = (u32) malloc(PAGE_SIZE * nr); 
	return tmp;
}

u32 kfree_page(struct page* t)
{
	if (t->pfn)
		free(t->pfn);
	free(t);
	return 0;

}

u32 free_page(struct page *t)
{
	if (t->pfn)
		free(t->pfn);
	free(t);
	return 0;
}
#endif

int copy_to_user(void *src, void *dest, u32 size)
{

}

int copy_from_user(void *dest, void *src, u32 size)
{

}
