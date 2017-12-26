#ifndef _ASM_H_HEAD_32
#define _ASM_H_HEAD_32
#define CR0_PG ( 1 << 31 )
#define KERNEL_OFFSET 0xc0000000
#define _pa(addr) (addr - KERNEL_OFFSET) 
#define task_esp 108
#define hw_info_addr 0x10000
#define hw_info_mem 0x1
#define hw_info_cpu 0x2
#endif


