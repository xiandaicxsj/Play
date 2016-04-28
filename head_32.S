/* * represent the address of certain funcu */
/* #include "head_32.h" */
.equ GDTNUM, 0x10
.equ _video_offset, 0xb8000
.equ IDT_ATTR, 0x8600
.equ mem_size, 0x200000 /* 2M */
.extern start_kernel

.macro push_all
	pushl %eax
	pushl %ebx
	pushl %ecx
	pushl %edx
	pushl %edi
	pushl %esi
.endm
 

.macro pop_all
	popl %esi
	popl %edi
	popl %edx
	popl %ecx
	popl %ebx
	popl %eax
.endm

#include "asm/head_32.h"
.section .data 
.align 64
.global gdt
gdt:
.fill 2, 8, 0
sys_code: 
.quad  0x00cf9a000000ffff
sys_data:	
.quad  0x00cf92000000ffff
sys_video:
.quad  0x0040920b8000ffff
default2:
.quad  0
.quad  0
.quad  0
.quad  0
.quad  0
.quad  0
.quad  0
.quad  0

gdt_base:
.word 16*8-1  /* gdt limit */
.long 0  /* gdt base  */

/* #include "head_32.h" */
.global sys_cs
.global sys_ds
sys_cs:
.word 0
sys_ds:
.word 0
.equ _kernel_cs, sys_code - gdt
.equ _kernel_ds, sys_data - gdt

.fill 4096, 1, 0
init_stack:

.fill 4096, 1, 0
.global init_page_dir
.align 4096
init_page_dir:
/* 4k */
/* 0xc[1100] 0000000 */
.fill 4096, 1, 0
init_page_tbl:
.fill 4096, 1, 0
	
/*;.globl make func available external
 ; loader: 1.make 16 bit ->32 bit 2. make it run into proteced model. should run into proteced model
 ; kernel: 1.will setup the pagingn 2.others do other basic initialition, kernel asm will setup man one-one map*/
.text	
//#include "kernel.lds.h"
#include "asm/page_type.h"
.align 32
.globl _start	
.type _start, @function
_start:
	//movl $_pa(init_stack), %esp
	//movl %esp, %ebp
	cli
	xorl %eax, %eax
	xorl %edi, %edi
	xorl %esi, %esi
	movl $_pa(gdt_base), %edi 
	movl $_pa(gdt), %eax
	movl %eax, 2(%edi)
	/* ecx is the size of kernel */
	lgdt _pa(gdt_base)

	/*jmp $_kernel_cs, $1f
1:      */
	movw $_kernel_ds, %ax
	movw %ax, %ds 
	movw %ax, %es 
	movw %ax, %gs 
	movw %ax, %fs 
	movw %ax, %ss 
	ljmp $_kernel_cs, $_pa(setup_paging)

setup_paging:
	movw $_kernel_cs, %ax;
	movw %ax, _pa(sys_cs) 
	movw $_kernel_ds, %ax;
	movw %ax, _pa(sys_ds) 
	movl $_pa(_bss_end), %ecx
	subl $_pa(_text_start), %ecx
	movl $mem_size, %ecx
	//movl $0x00300000, %ecx

	movl %ecx, %edx
	movl %ecx, %eax
	shrl $22, %ecx

	andl $0x003fffff, %eax
	cmp %eax, 0
	je 3f
	incl %ecx
3:
	movl $_pa(init_page_tbl), %eax
	orl  $PGT_P, %eax
	orl  $PGT_U, %eax
/* nowday we use user page to test */
//	orl  $PGT_S, %eax
	orl  $PGT_W, %eax
	movl $_pa(init_page_dir), %edi
1:
	movl %eax, 0xc00(%edi)
	stosl
	addl $4096, %eax
	decl %ecx
	jnz 1b


	movl %edx, %ecx
	andl $0x003fffff, %ecx
	shrl $12, %ecx
	xorl %eax, %eax
	orl  $PGT_P, %eax
	//orl  $PGT_S, %eax
	orl  $PGT_U, %eax
	orl  $PGT_W, %eax
	movl $_pa(init_page_tbl), %edi
2: 
	stosl
	addl $4096, %eax
	decl %ecx
	jnz 2b


	movl $_pa(init_page_dir), %eax
	movl %eax, %cr3
	movl %cr0, %eax
	movl %cr4, %edx
	orl  $0x80000000, %eax
	movl %eax, %cr0
	movl $init_stack, %esp
	movl %esp, %ebp
	ljmp $_kernel_cs, $1f	
1:
	jmp init_pic_as
init_pic_as:
	movb $0x11, %al
	outb %al, $0x20
	nop
	nop
	nop
	nop
	outb %al, $0xa0
	nop
	nop
	nop
	nop
	
	movb $0x20, %al
	outb %al, $0x21
	nop
	nop
	nop
	nop
	movb $0x28, %al
	outb %al, $0xa1
	nop
	nop
	nop
	nop

	
	movb $0x4, %al
	outb %al, $0x21
	nop
	nop
	nop
	nop
	movb $0x2, %al
	outb %al, $0xa1
	nop
	nop
	nop
	nop
	
	movb $0x1, %al
	outb %al, $0x21
	nop
	nop
	nop
	nop
	outb %al, $0xa1
	nop
	nop
	nop
	nop
	movb $0xfe, %al
	outb  %al, $0x21
	nop
	nop
	nop
	nop
	movb $0xff, %al
	outb  %al, $0xa1
	nop
	nop
	nop
	nop
	call start_kernel
	
.type _default_inter_handler, @function
_default_inter_handler:
	/*pushl %esp
	movl %esp, %ebp

	movl %ebp, %esp
	popl %esp
	*/
	ret

/*.type _setup_idt,@function
_setup_idt:

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
	movl %eax, idt(%ecx,%edi,2)
	movl %edx, idt(%ecx,%edi,2) 
	addl $1, %edi
	subl $1, %ecx
	jnz 1b

	xorl %eax, %eax
	movl $idt, %eax
	movl $idt_base, %edi
	movl %eax, 2(%edi)
	lidt idt_base
	sti

	ret
*/
.global hw0
.global hw1
.global hw2
.global hw3
.global hw4
.global hw5
.global hw6
.global hw7
.global hw8
.global hw9
.global hw10
.global hw11
.global hw12
.global hw13
.global hw14
.global hw15
.global hw32
.global hw33
.global hw34
.global hw35
.global hw36
.global hw37
.global hw38
.global hw39
.global hw40
.global hw44
.global hw45
.global hw46
.global hw48

.type hw0, @function
hw0:
	push_all
	call divide_error
	pop_all
 	iret
 
.type hw1, @function
hw1:
	push_all
	call single_step_exception
	pop_all
	iret
 
	
.type hw2, @function
hw2:
	push_all
	call nmi
	pop_all
	iret
 
.type hw3, @function
hw3:
	push_all
	call breakpoint_exception
	pop_all
	iret

.type hw4, @function
hw4:
	push_all
	call overflow
	pop_all
	iret

.type hw5, @function
hw5:
	push_all
	call bounds_check
	pop_all
	iret

.type hw6, @function
hw6:
	push_all
	call inval_opcode
	pop_all
	iret
 
.type hw7, @function
hw7:
	push_all
	call copr_not_available
	pop_all
	iret
 
.type hw8, @function
hw8:
	push_all
	call double_fault
	pop_all
	iret

.type hw9, @function
hw9:
	push_all
	call copr_seg_overrun
	pop_all
	iret

.type hw10, @function
hw10:
	push_all
	call inval_tss 
	pop_all
	iret

.type hw11, @function
hw11:
	push_all
	call segment_not_present
	pop_all
	iret

.type hw12, @function
hw12:
	push_all
	call stack_fault
	pop_all
	iret

.type hw13, @function
hw13:
	push_all
	call general_protection
	pop_all
	iret

.type hw14, @function
hw14:
	push_all
	call page_fault
	pop_all
	iret

.type hw15, @function
hw15:
	push_all
	iret
	pop_all

.type hw32, @function
hw32:
	push_all
	call timer_handler
	pop_all
	iret

.type hw33, @function
hw33:
	iret

.type hw34, @function
hw34:
	iret

.type hw35, @function
hw35:
	iret
.type hw36, @function
hw36:
	iret

.type hw37, @function
hw37:
	iret

.type hw38, @function
hw38:
	iret

.type hw39, @function
hw39:
	iret

.type hw40, @function
hw40:
	iret

.type hw44, @function
hw44:
	iret

.type hw45, @function
hw45:
	iret

.type hw48, @function
hw48:
	cli
	push_all
	call handle_user_int
	pop_all
	sti
	iret

