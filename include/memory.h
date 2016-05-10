#ifndef H_MEMORY
#define H_MEMORY
#include"type.h"
#define MEM_KERN (1 << 0)
#define MEM_USER (1 << 1)
void setup_memory();
void* kmalloc(u32 size, u32 align);
#endif
