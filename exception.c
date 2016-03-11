#include "exception.h"
//#include "schdule.h"
#include "gate.h"
#include "page.h"
struct idt_desc {
	u32 low;
	u32 high;
};
struct idt_base {
	u8 limit;
	u32 idt_addr;
};
extern u32 sys_cs;
static struct idt_desc idt[256] ;
u32 array[200]={0};
static struct idt_base idt_base ;

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
}
void copr_error()
{

}

void setup_idt()
{
        set_idt(DEBUG, single_step_exception, DA_386IGate);
        set_idt(NMI, nmi, DA_386IGate);
        set_idt(BREAKPOINT, breakpoint_exception,DA_386IGate);
        set_idt(OVERFLOW, overflow, DA_386IGate);
        set_idt(BOUND_RANGE_EXCEDD,bounds_check,DA_386IGate);
        set_idt(INVALID_OP,inval_opcode,DA_386IGate);
        set_idt(DEVICE_NOT_AVAL,copr_not_available,DA_386IGate);
        set_idt(DOUBL_FAULT,double_fault,DA_386IGate);
        set_idt(COP_SEGMENT,copr_seg_overrun,DA_386IGate);
        set_idt(IVALID_TSS,inval_tss,DA_386IGate);//这里修改了
        set_idt(SEGMENT_NOT_PRESETNT,segment_not_present, DA_386IGate);
        set_idt(STACK_FAULT,stack_exception, DA_386IGate);
        set_idt(GENERAL_FAULT, general_protection, DA_386IGate);
        set_idt(PAGE_FAULT, page_fault, DA_386IGate);
        set_idt(TIMER, timer_handler,DA_386IGate);
	load_idt();
}
