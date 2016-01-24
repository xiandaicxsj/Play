/*
 * represent the address of certain func
 */
/* #include "head_32.h" */
.equ GDTNUM, 128 
.equ KERNRL_DS, 0x1
.equ kERNEL_CS, 0x2
.equ IDT_ATTR, 0x8600
.extern start_kernel

.section .data 
.align 32
gdt:
.quad 0
sys_code: 
.quad  0x004f98000000ffff
sys_data:	
.quad  0x004f92000000ffff
default2:
.quad  0
.quad  0
.quad  0
.quad  0
.quad  0

gdt_base:
.short 0  /* gdt limit */
.long 0   /* gdt base  */

idt:
.fill 4048, 8, 0 

idt_base: 
.short 0
.long 0

init_stack:
.fill 4048, 8, 0

/*;.globl make func available external
 ; loader: 1.make 16 bit ->32 bit 2. make it run into proteced model. should run into proteced model
 ; kernel: 1.will setup the pagingn 2.others do other basic initialition, kernel asm will setup man one-one map*/
.text	
#include "head_32.h"
.align 32
.globl _start	
.type _start, @function
_start:
	xorl %eax,%eax
	movw %ax,%ds 
	movw %ax,%cs 
	jmp start
start:
	movl $_pa(init_stack), %esp
	xorl  %eax,%eax
	xorl  %edi,%edi
	movl $_pa(gdt),%eax  
	movl $_pa(gdt_base),%edi
	movl %eax, 2(%edi)
	lgdt _pa(gdt_base)
	
	call _setup_idt
	call _setup_paging
	call start_kernel

.type _default_inter_handler, @function
_default_inter_handler:
	/*pushl %esp
	movl %esp, %ebp

	movl %ebp, %esp
	popl %esp
	*/
	ret

.type _setup_idt,@function
_setup_idt:

	/* pushl %esp */
	/* movl %esp, %ebp */
	cli
	1:
	movl $21, %ecx
	movl $0, %edi
	xorl %eax, %eax
	xorl %edx, %edx
	movl $_default_inter_handler,%eax
	andl 0x0000ffff, %eax
	movl $0x1, %edx
	shl  $32, %edx 
	orl %edx, %eax
	xorl %edx, %edx
	movl $_default_inter_handler,%edx
	andl $0xffff0000, %edx
	movw $IDT_ATTR, %dx 
	movl %eax, idt(%ecx,%edi,2) /* _idt(1,) */
	movl %edx, idt(%ecx,%edi,2) /* _idt(2,) */
	addl $1, %edi
	subl $1, %ecx
	jnz 1b

	xorl %eax, %eax
	movl $idt, %eax
	movl $idt_base, %edi
	movl %eax, 2(%edi)
	lidt idt_base
	sti

	/*
	movl %ebp, %esp
	popl %esp
	*/
	ret
	
.type _setup_paging,@function
_setup_paging:
	pushl %ebp
	pusha
	movl %esp, %ebp
	movl %cr0, %eax
	orl  $CR0_PG, %eax
	movl %eax, %cr0
	movl %ebp, %esp
	popa
	popl %esp
	ret

	



