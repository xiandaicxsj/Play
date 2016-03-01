#include"gate.h"
#include"kernel.h"
#include"global.h"
void init_task()
{
	set_tss(TSS_FIRST,&(g_task[0].task_reg));
	set_ldt(LDT_FIRST,&(g_task[0].ldt),0xffffffff);
	set_gdt_seg((void *)(&g_task[0].ldt),0,0,DA_C|DA_DPL3,0xffffffff);
	set_gdt_seg((void *)(&g_task[0].ldt),1,0,DA_DRW|DA_DPL3,0xffffffff);
	g_task[0].task_reg.previous_link=0;
	g_task[0].task_reg.esp0=top_of_stack;
	g_task[0].task_reg.ss0=KERNEL_DATA;//这里还是需要在提交的
	g_task[0].task_reg.esp1=0;
	g_task[0].task_reg.ss1=0;
	g_task[0].task_reg.esp2=0;
	g_task[0].task_reg.ss2=0;
	g_task[0].task_reg.cr3=0;//这里没有开启分也应不需要设置吧
	g_task[0].task_reg.eip=(u32) process;
	g_task[0].task_reg.eflags=0x1202;//这里还没有确定如何设置
	g_task[0].task_reg.eax=0;
	g_task[0].task_reg.ebx=0;
	g_task[0].task_reg.edx=0;
	g_task[0].task_reg.ecx=0;
	g_task[0].task_reg.esi=0;
	g_task[0].task_reg.edi=0;
	g_task[0].task_reg.esp=(u32)task_ss+TASK_SS;
	g_task[0].task_reg.ebp=0;
	g_task[0].task_reg.es=0x08|SA_TIL|SA_RPL3;//下面的数字都是代表的出现在ldt中的便宜中的
	g_task[0].task_reg.cs=0x00|SA_TIL|SA_RPL3;
	g_task[0].task_reg.fs=0x08|SA_TIL|SA_RPL3;
	g_task[0].task_reg.gs=0x08|SA_TIL|SA_RPL3;
	g_task[0].task_reg.ds=0x08|SA_TIL|SA_RPL3;
	g_task[0].task_reg.ss=0x08|SA_TIL|SA_RPL3;
	g_task[0].task_reg.ldt=LDT_FIRST*8;
	g_task[0].task_reg.io=0;
	g_task[0].task_reg.tss=TSS_FIRST*8;
	LDT_FIRST+=2;
	TSS_FIRST+=2;
}

static void next_task()
{
	if(current_task_num==TASK_NUM-1)
	{
		current_task_num=0;
	}
	else current_task_num++;
	print_int(current_task_num);
	current_task=&(g_task[current_task_num]);
}
void switch_task()
{
		struct {u32 a,b;} __tmp;
		__asm volatile ("movw  %%dx, %1\n\t" // why __asm can work
			      "ljmp %0 \n\t" 
			      ::"m"(*&__tmp.a), "m"(*&__tmp.b), 
			      "d"(current_task->task_reg.tss));
}
