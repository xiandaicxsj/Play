#ifndef H_MEMORY
#define H_MEMORY
#include"type.h"
#include"page.h"
#define MEM_KERN (1 << 0)
#define MEM_USER (1 << 1)
void setup_memory();
struct page *alloc_page(u32 flags);
struct page *alloc_pages(u32 nr, u32 flags);
u32 free_page(struct page*);
void* kmalloc(u32 size, u32 align, u32 flags); /* do we need flags */
u32 kfree(void *);


#endif
