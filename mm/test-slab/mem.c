#include"mem.h"
#include<stdio.h>
#include<stdlib.h>

struct page *kalloc_page(u32 flags)
{
	struct page *pg = malloc(sizeof(*pg));
	pg->pfn = (u32) malloc(PAGE_SIZE);
}

u32 free_page(struct page *pg)
{
	free((void *)pg->pfn);
	free((void *)pg);
}


