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
	setup_interrupt();
	setup_memory();
	/*
	 *init_vfs();
	 *pre_init_devices();
	 *init_fs();
	 *post_init_devices();
	 */

			
	init_schduler();
	struct task_struct * task0 = (struct task_struct *) kmalloc(PAGE_SIZE, 0, MEM_KERN); /* should be page align */
	create_task(task0, test_process);


	struct task_struct * task1 = (struct task_struct *) kmalloc(PAGE_SIZE, 0, MEM_KERN); /* should be page align */
	create_task(task1, test_process1);
	/* test map page */ 
	//map_page(0xcffc0, 0, MEM_KERN, &init_page_dir);
	/*
	 *	init_console();
	 */

	switch_to_test(task0);
	while(1);
	setup_interrupt();

	while(1);
	return ;
}

