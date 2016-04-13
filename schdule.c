#include"schdule.h"
#include"gate.h"
#include"kernel.h"
#include"global.h"
#include"page.h"
#include"string.h"
#include"debug.h"
#define TASK_SS 4048
#define HW_TASK_SWITCH
struct task_head_t
{
	struct task_struct *task_list;
	u32  task_num;
};
static struct task_head_t task_head;
static void insert_task(struct task_struct *_task)
{
	if (!task_head.task_list)
	{
		task_head.task_list = _task;
		return ;
	}
	_task->next = task_head.task_list;
	task_head.task_list = _task;
	return; 
}
void test_process()
{
	while(1);
}

void switch_to_test()
{

	/*struct tmp{
	unsigned int a;
	unsigned int b;
	};*/
	struct tmp{
	long a;
	long b;
	};
	unsigned int task_vec;
	struct tmp _t;
	_t.b = (TASK_VECTOR  << 3);
	//_t.b = TASK_VECTOR;
	task_vec = _t.b ;
	//asm volatile(/*"  movw %0, %%ax\n\t"
	//	           "  movw %1, %%dx\n\t"*/
	//	           "ltr %%ax"::"a"(task_vec), "m" (sys_ds) :);

	
	//asm volatile(" ljmp %0 ":: "m" (ask_vec):);
	asm volatile(" ljmp %0 ":: "m" (*&_t.a):);
	

}
void pre_init_task(void )
{
	/*unsigned int  task_vec = TASK_VECTOR_EMP << 3;
	set_tss(TASK_VECTOR_EMP, 0); 
	asm volatile ("ltr %%ax"::"a"(task_vec) :);
	while(1);
	*/
}
/*
 *
 * ss[0-3] static
 * ss/cs/ds/eip/should be set.
 */
void init_task(struct task_struct *task)
{
	u32 sys_ds = (0x18 );
	u32 sys_cs = (0x10 );
	memset(task, 0, sizeof(*task));
	task->task_reg.ss0 = sys_ds;
	task->task_reg.esp0 = (u32) task + PAGE_SIZE - 1 ;

	task->task_reg.cr3 = virt_to_phy((u32)&init_page_dir);  
	task->task_reg.eip = (u32) test_process;
	task->task_reg.esp = (u32) task + PAGE_SIZE - 1;

	task->task_reg.es = sys_ds;
	task->task_reg.cs = sys_cs;
	task->task_reg.ds = sys_ds;
	task->task_reg.fs = sys_ds;
	task->task_reg.gs = sys_ds;
	task->task_reg.ss = sys_ds;
	task->task_reg.ldt = 0;
	
	insert_task(task);
	set_tss(TASK_VECTOR, task); 
	switch_to_test();
}

void switch_to(struct task_struct *pre, struct task_struct *next)
{
	/*  asm volatile ("lgdt \n\t"
								"\n\t"
								:[p_esp] =m ""
			);
			*/
}

