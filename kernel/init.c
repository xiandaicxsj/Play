#include"mem.h"
#include"tty.h"
#include"exception.h"
#include"schdule.h"
#include"page.h"
#include"debug.h"
#define TEST_TASK
void start_kernel(void )
{
	setup_memory();

	setup_interrupt();

	struct task_struct * task0 = (struct task_struct *) kmalloc(PAGE_SIZE, 0, MEM_KERN); /* should be page align */
	init_task(task0);

	struct task_struct * task1 = (struct task_struct *) kmalloc(PAGE_SIZE, 0, MEM_KERN); /* should be page align */
	init_task(task1);
	switch_to_test(task0);

	init_console();
	while(1);
	return ;
}

