#include"schdule.h"
#include"gate.h"
#include"kernel.h"
#include"global.h"
#include"page.h"
#include"string.h"
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

	struct tmp{
	long a;
	long b;
	};
	struct tmp _t;
	_t.b = TASK_VECTOR;
	asm volatile(" ljmp %0 ":: "m" (_t.a):);
}
void init_task(struct task_struct *task)
{
	//* task d
	memset(task, 0, sizeof(*task));
	task->task_reg.eip = (u32) test_process;
	task->task_reg.esp0 = (u32) task + PAGE_SIZE - 1 ;
	task->task_reg.cr3 = PHY_ADDR((u32)init_page_dir);  
	insert_task(task);
	set_tss(TASK_VECTOR, &(task->task_reg)); 
	while(1);
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

