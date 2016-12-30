#include"math.h"
#include"mem.h"


#include<stdio.h>
#include<stdlib.h>

#define MAX_ORDER 3
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

struct page *pfn_to_page(u32 pfn)
{
	return pages_list + pfn;
}

static void add_area_pages(struct free_area_t *area, struct page *pages)
{
	if(!area->free_pages) {
		/*suppose*/
		area->free_pages = pages;
	} else
		list_add(&(pages->list), &(area->free_pages->list));
	area->nr_free ++;
}

void init_buddy(u32 mem_size)
{
	u32 order;
	u32 nr_pages;
	u32 pages_idx;
	u32 size;
	struct free_area_t *area;
	struct page *pages;

	order = 0;
	nr_pages = mem_size >> PAGE_SHIFT;

	while (order <= MAX_ORDER) {
		area = &(pgp.free_area[order]);
		area->nr_free = 0;
		area->order = order;
		area->free_pages = NULL;
		order ++;
	}

	order = MAX_ORDER;
	area = &(pgp.free_area[order]);

	/*FIXME */
	pages_idx = 0;
	size = 1  << order;

	while(pages_idx < nr_pages) {

		pages = pfn_to_page(pages_idx);
		pages->order = order;
		add_area_pages(area, pages); 

		pages_idx += size;
	}

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
	if (lo_order == hi_order)
		goto out;

	while(lo_order < hi_order) {
		area --;
		hi_order --;
		size >>= 1;

		tmp_pages = &pages[size];
		tmp_pages->order = hi_order;

		add_area_pages(area, tmp_pages);
	}
out:
	pages->order = lo_order;
	pages->count ++;
	return pages;
}

/* 
 * the order or the size of certain pages is store in 
 * the first page of the pagses.
 */
static u32 check_buddy(struct page *pages, struct page *buddy, u32 order)
{
	/* FIXME */
	if(buddy->order == order && !buddy-> count)
		return 1;
	return 0;
}

static u32 del_area_pages(struct free_area_t *area, struct page *pages)
{
	/* ASSERT(!area->nr_free); */
	area->nr_free--;
	if (!area->nr_free)
		area->free_pages = NULL;
	else 
		area->free_pages = container_of(pages->list.next, struct page, list);
	list_del(&(pages->list));
	/* we need this reset this port to itself*/
	list_init(&(pages->list));
}

static zero_page_order(struct page *pages)
{
	pages->order = -1;
}

static void _buddy_free_pages(struct page *pages)
{
	struct  free_area_t *area;
	struct page *buddy_pages;
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

	pages->count --;
	while(order <= MAX_ORDER) {
		area = &(pgp.free_area[order]);
		page_idx = pages->pfn;

		buddy_idx = get_buddy_idx(page_idx, order);
		buddy_pages = pfn_to_page(buddy_idx);

		/*FIXME */
		if (!check_buddy(pages, buddy_pages, order) || order == MAX_ORDER) {
			pages->order = order;
			add_area_pages(area, pages);
			break;
		}

		zero_page_order(pages);	

		zero_page_order(buddy_pages);	
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
	u32 hi_order = lo_order;
	struct page *pages;
	struct free_area_t *area;
	/*FIXME */
	while(hi_order <= MAX_ORDER) {
		area = &(pgp.free_area[hi_order]);
		/*FIXME */
		if (hi_order == MAX_ORDER && (!area->nr_free))
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
		del_area_pages(area, pages);

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
*/
void show_area()
{
	u32 idx = 0;
	struct free_area_t *area;

	printf("++++++++++++\n");
	for(;idx <= MAX_ORDER; idx++) {
		area = &pgp.free_area[idx];
		printf("area order: %d free pages %d\n", idx, area->nr_free);
	}
}
void show_buddy_info(u32 size)
{
	int pages = size >> PAGE_SHIFT;
	int pfn = 0;
	struct page *page;

	printf("-----------------------\n");
	while(pfn < pages) {
		page = pfn_to_page(pfn);
		printf("pfn: %d -- order: %d -- count: %d\n", page->pfn, page->order, page->count);
		pfn ++;
	}
	show_area();
	printf(">>>>>>>>>>>>>>>>>>>>>\n\n");
	/*
	int order = MAX_ORDER -1;
	while(order >= 0)
	{
		show_buddy_area(order);
		order --;
	}
	*/
}

static struct page * _buddy_alloc(u32 size, u32 align)
{
	/* align is useless */
	u32 nr_pages = size >> PAGE_SHIFT;
	nr_pages += (size & PAGE_MASK ? 1 : 0);
	return _buddy_alloc_pages(nr_pages);
}

u32 static get_mem_size()
{
	/* here we juest write */
	return 8*1024*4;
}
/* now the memory is not set properily */


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

void show_page_buddy_info(struct page *pages)
{
	printf("pages-order: 0x%d\n", pages->order);
}

int main()
{
	struct page *pages;
	struct page *pages0;
	struct page *pages2;
	struct page *pages3;
	struct page *pages31;
	u32 mem_size = get_mem_size();
	init_pages_list(mem_size);
	init_buddy(mem_size);
	show_buddy_info(mem_size);

	pages0 = _buddy_alloc_pages(1);
	show_buddy_info(mem_size);
	pages2 = _buddy_alloc_pages(1);
	show_buddy_info(mem_size);
	_buddy_free_pages(pages0);
	show_buddy_info(mem_size);
	_buddy_free_pages(pages2);
	show_buddy_info(mem_size);

	pages2 = _buddy_alloc_pages(2);
	show_buddy_info(mem_size);
	//show_buddy_info();
	pages3 = _buddy_alloc_pages(1);
	show_buddy_info(mem_size);
	//show_buddy_info();
	_buddy_free_pages(pages2);
	show_buddy_info(mem_size);
	_buddy_free_pages(pages3);
	show_buddy_info(mem_size);

	pages2 = _buddy_alloc_pages(3);
	show_buddy_info(mem_size);
	pages3 = _buddy_alloc_pages(6);
	if (!pages3)
		printf("not enough memory \n");
	show_buddy_info(mem_size);

	//show_buddy_info();
	return 0;
}
