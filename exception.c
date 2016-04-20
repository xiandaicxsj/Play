#include "exception.h"
#include "gate.h"
#include "page.h"
#include "pic.h"
#include "print.h"
struct idt_base_t {
	u16 size;
	u32 addr;
}__attribute__((packed));
u32 array[200]={0};
extern u32 sys_cs;
static struct idt_desc idt[256] ;
static struct idt_base_t idt_base ;

void default_handler1()
{

}

void divide_error()
{

}

void single_step_exception()
{

}

void nmi()
{
}

void breakpoint_exception()
{
}

void overflow()
{
}

void timer_handler()
{
	disable_interrupt();
	while(1);
	asm volatile("iret");
}

void bounds_check()
{

}

void inval_opcode()
{
}
void copr_not_available()
{
}
void double_fault()
{
}
void copr_seg_overrun()
{
}
void inval_tss()////
{
}

void segment_not_present()
{
}

void stack_exception()
{
}

void general_protection()
{
	print_str("gp fault\n");
	print_str("a\n");
	print('b');
	while(1);
}
void copr_error()
{

}

static void load_idt()
{
	idt_base.addr = (u32) &(idt[0]);
	idt_base.size = 0xffff;
	asm volatile ( "lidt %0" ::"m"(idt_base) );
}

void setup_interrupt()
{

	init_pic();

	set_idt(idt, DEBUG, single_step_exception, DA_386IGate);
	set_idt(idt, NMI, nmi, DA_386IGate);
	set_idt(idt, BREAKPOINT, breakpoint_exception,DA_386IGate);
	set_idt(idt, OVERFLOW, overflow, DA_386IGate);
	set_idt(idt, BOUND_RANGE_EXCEDD,bounds_check,DA_386IGate);
	set_idt(idt, INVALID_OP,inval_opcode,DA_386IGate);
	set_idt(idt, DEVICE_NOT_AVAL,copr_not_available,DA_386IGate);
	set_idt(idt, DOUBL_FAULT,double_fault,DA_386IGate);
	set_idt(idt, COP_SEGMENT,copr_seg_overrun,DA_386IGate);
	set_idt(idt, IVALID_TSS,inval_tss,DA_386IGate);//这里修改了
	set_idt(idt, SEGMENT_NOT_PRESETNT,segment_not_present, DA_386IGate);
	set_idt(idt, STACK_FAULT,stack_exception, DA_386IGate);
	set_idt(idt, GENERAL_FAULT, general_protection, DA_386IGate);
	set_idt(idt, PAGE_FAULT, page_fault, DA_386IGate);
	set_idt(idt, TIMER, timer_handler,DA_386IGate);
	load_idt();
	enable_interrupt();
}
