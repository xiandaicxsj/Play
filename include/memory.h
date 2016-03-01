#ifndef H_MEMORY
#define H_MEMORY
#include"type.h"
void setup_memory();
void* kmalloc(u32 size, u32 align);
#endif
