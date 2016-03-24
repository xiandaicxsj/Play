#include"schdule.h"
#include"gate.h"
#include"kernel.h"
#include"global.h"
#define TASK_SS 4048
#define HW_TASK_SWITCH
struct task * task_list;
struct task_head_t
{
	struct task *task_list;
	u32  task_num;
};
static struct task_head_t task_head;
static void insert_task(struct task *_task)
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
void init_task(struct task *_task)
{
	//* task d
	_task->ldt = ; 
	_task->previous_link = ;
	_task->io = 0; 
	_task->cr3 = (u32) alloc_page_table();
	insert_task(_task);
}
void switch_task()
{

}
