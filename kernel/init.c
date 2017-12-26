#include"mem.h"
#include"tty.h"
#include"exception.h"
#include"schdule.h"
#include"page.h"
#include"debug.h"
#include"vfs.h"
#include"test_process.h"
#include"timer.h"
#include"print.h"
#include"irq.h"
void start_kernel(void )
{
	struct task_struct * task0;
	//struct task_struct * task1;
	setup_memory();
	local_irq_disable();
	setup_interrupt();

	/*
	 *init_vfs();
	 *pre_init_devices();
	 *init_fs();
	 *post_init_devices();
	 *init_console();
	 */

	init_schduler();

	init_timer();
	create_idle_ktask();
	task0 = (struct task_struct *) create_task(NULL, test_process, 0);
	switch_to_ring3(task0);
	local_irq_disable();
	while(1);
}

