#ifndef _H_DEBUG
#define _H_DEBUG
#define DEBUG_REGS(eax, ebx, ecx, edx) \
	asm volatile ( "movl  %%eax, %%eax"::"a"(eax), "b"(ebx), "c"(ecx), "d"(edx):);
#endif
