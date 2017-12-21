#ifndef H_MEM
#define H_MEM
#include"type.h"
#include"page.h"
#define MEM_SUPER (1 << 0)
#define MEM_USER (1 << 1)
#define MEM_READ (1 << 2)
#define MEM_RDWR (1 << 3)
#define MEM_NO_MAP (1<< 4)
#define MEM_KERN (MEM_RDWR | MEM_SUPER)
struct page *kalloc_page(u32 flags);
u32 free_page(struct page*);
#endif
