#include "exception.h"
#include "gate.h"
#include "page.h"
#include "pic.h"
#include "print.h"
#include "head_32.h"
#define setup_interrupt_handler(idt, index) \
	do { \
		set_idt(idt, index, HW_VC(index), DA_386IGate, 0); \
	} while(0) 	
struct idt_base_t {
	u16 size;
	u32 addr;
}__attribute__((packed));
u32 array[200]={0};
extern u32 sys_cs;
static struct idt_desc idt[256] ;
static struct idt_base_t idt_base ;

typedef void (*irq_handler)(void);
irq_handler _irq[255];

void divide_error();
void single_step_exception();
void nmi();
void breakpoint_exception();
void overflow();
void bounds_check();
void inval_opcode();
void copr_not_available();
void double_fault();
void copr_seg_overrun();
void inval_tss();
void segment_not_present();
void stack_exception();
void general_protection();
void copr_error();

void disable_interrupt()
{
	                asm volatile ( "cli" );
}

void enable_interrupt()
{
	                asm volatile ( "sti" );
}


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
//	print_str("timer\n");
	send_eoi(TIMER);
	test_switch_task();
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

void stack_fault()
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

void handle_user_int(u32 index)
{
	print_str("user int\n");
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
	setup_interrupt_handler(idt, DEBUG);
	setup_interrupt_handler(idt, NMI);
	setup_interrupt_handler(idt, BREAKPOINT);
	setup_interrupt_handler(idt, OVERFLOW);
	setup_interrupt_handler(idt, BOUND_RANGE_EXCEDD);
	setup_interrupt_handler(idt, INVALID_OP);
	setup_interrupt_handler(idt, DEVICE_NOT_AVAL);
	setup_interrupt_handler(idt, DOUBL_FAULT);
	setup_interrupt_handler(idt, COP_SEGMENT);
	setup_interrupt_handler(idt, IVALID_TSS);//这里修改了
	setup_interrupt_handler(idt, SEGMENT_NOT_PRESETNT);
	setup_interrupt_handler(idt, STACK_FAULT);
	setup_interrupt_handler(idt, GENERAL_FAULT);
	setup_interrupt_handler(idt, PAGE_FAULT);
	setup_interrupt_handler(idt, TIMER);
	/* set_up int used for usespace application */
	set_idt(idt, INT_USER, HW_VC(48), DA_386TGate, 3);
	load_idt();
	enable_interrupt();
}
