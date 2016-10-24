#include"memory.h"
#include"tty.h"
#include"exception.h"
#include"schdule.h"
#include"page.h"
#define TEST_TASK
void start_kernel(void )
{
	setup_memory();
	setup_idt();
	struct task_struct * task0 = (struct task_struct *) kmalloc(PAGE_SIZE, PAGE_SIZE); /* should be page align */
	pre_init_task();
	init_task(task0);
	init_console();
	while(1);
	return ;
}
