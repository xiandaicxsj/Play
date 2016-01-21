#ifndef _H_HEAD_32
#define _H_HEAD_32
#define CR0_PG ( 1 << 31 )
#define KERNEL_OFFSET 0xc000000
#define _pa(addr) (addr - KERNEL_OFFSET)
#define PAGE_SIZE ( 1 << PAGE_OFFSET ) 
#endif


