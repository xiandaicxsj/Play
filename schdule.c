#include"schdule.h"
#include"gate.h"
#include"kernel.h"
#include"global.h"
#define TASK_SS 4048

void init_task(struct task *_task)
{
	//* task d
	_task->ldt = ; 
	_task->cr3 = ; 
	_task->previous_link = ;
	_task->io = 0; 
	_task->cr3 = (u32) alloc_page_table();
}
void switch_task()
{

}
