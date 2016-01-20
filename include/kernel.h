#ifndef _H_KERNEL
#define _H_KERNEL
/*
 * this header is used export function from kernel asm
 */
extern void switch_to1(struct tss_reg *);
extern void print_int(int k);
extern void print(char *);
extern void enable_interrupt();
#endif
