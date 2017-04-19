#include"mem.h"
#include"tty.h"
#include"exception.h"
#include"schdule.h"
#include"page.h"
#include"debug.h"
#include"vfs.h"
#include"test_process.h"
void start_kernel(void )
{
	struct task_struct * task0;
	setup_interrupt();
	setup_memory();
	/* disable irq */
	local_irq_enable();

	/*
	 *init_vfs();
	 *pre_init_devices();
	 *init_fs();
	 *post_init_devices();
	 *init_console();
	 */

	init_schduler();
	task0 = (struct task_struct *) create_task(NULL, test_process, 0);

	/* local_irq_enable(); */
	switch_to_ring3(task0);
	local_irq_disable();
	while(1);
}

