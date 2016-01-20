#include"schdule.h"
#include"global.h"
#include"memory.h"
#include"page.h"
#include"exception.h"
#include"gate.h"
#define TASK_SS 4048


//u16 KERNEL_CODE=0x08;
//u16 KERNEL_DATA=0x10;
char task_ss[TASK_SS];
extern int top_of_stack;
extern int print(char *s);
extern void enable_timer();
extern void enable_interrupt();
void print_int(int num)
{
	char buffer[32];
	char buffer2[32];
	int pos=0;
	int left=0;
	while(num/16!=0)
	{
		left=num%16;
		if(left<10)
		{
			buffer[pos]=(char)(left+48);
			pos++;
		}
		else 
		{
			buffer[pos]=(char)(left-10+'a');
			pos++;
		}
		num=num/16;
	}
	if(num<10)
		buffer[pos]=(char)(num+48);
	else
		buffer[pos]=(char)(num-10+'a');
	int i=0;
	for(i=0;i<=pos;i++)
		buffer2[i]=buffer[pos-i];
	buffer2[pos+1]='\0';
	print(buffer2);
	
}
void init_idt()
{
	set_idt(DIEVIDE_ERROR,divide_error,DA_386IGate);
	set_idt(DEBUG,single_step_exception,DA_386IGate);
	set_idt(NMI,nmi,DA_386IGate);
	set_idt(BREAKPOINT,breakpoint_exception,DA_386IGate);
	set_idt(OVERFLOW,overflow,DA_386IGate);
	set_idt(BOUND_RANGE_EXCEDD,bounds_check,DA_386IGate);
	set_idt(INVALID_OP,inval_opcode,DA_386IGate);
	set_idt(DEVICE_NOT_AVAL,copr_not_available,DA_386IGate);
	set_idt(DOUBL_FAULT,double_fault,DA_386IGate);
	set_idt(COP_SEGMENT,copr_seg_overrun,DA_386IGate);
	set_idt(IVALID_TSS,inval_tss,DA_386IGate);//这里修改了
	set_idt(SEGMENT_NOT_PRESETNT,segment_not_present,DA_386IGate);
	set_idt(STACK_FAULT,stack_exception,DA_386IGate);
	set_idt(GENERAL_FAULT,general_protection,DA_386IGate);
	set_idt(PAGE_FAULT,page_fault,DA_386IGate);
	set_idt(TIMER,timer_handler,DA_386IGate);
	return ;
}
void init()
{
//	idt_base_dec *i_base=(idt_base_dec *)&idt_base;
//	i_base->length=(u16)256;
//	i_base->base=(u32)idt_dec;
	//print("kernel begin");
	//init_idt();
	init_memory();
	init_task();
}
