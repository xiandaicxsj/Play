#ifndef H_MEM
#define H_MEM
#include"type.h"
#include"page.h"
#include"test.h"
#define MEM_SUPER (1 << 0)
#define MEM_USER (1 << 1)
#define MEM_READ (1 << 2)
#define MEM_RDWR (1 << 3)
#define MEM_NO_MAP (1<< 4)
#define MEM_KERN (MEM_RDWR | MEM_SUPER)
void setup_memory();
struct page *kalloc_page(u32 flags);
struct page *kalloc_pages(u32 nr, u32 flags);
u32 free_page(struct page*);
void* kmalloc(u32 size, u32 align, u32 flags); /* do we need flags */
u32 kfree(void *);
u32 kfree_page(struct page* page);
int copy_to_user(void *src, void *dest, u32 size);
int copy_from_user(void *dest, void *src, u32 size);

#endif
