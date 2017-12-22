#include"mem.h"
#include<stdio.h>
#include<stdlib.h>
#include"list.h"

struct page *kalloc_page(u32 flags)
{
	struct page *pg = malloc(sizeof(*pg));
	pg->pfn = malloc(PAGE_SIZE);
	list_init(&pg->list);
	return pg;
}

u32 free_page(struct page *pg)
{
	free((void *)pg->pfn);
	free((void *)pg);
}


