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
	/*
	 *init_vfs();
	 *pre_init_devices();
	 *init_fs();
	 *post_init_devices();
	 */
			
	init_schduler();

	task0 = create_task(NULL, test_process);

	//struct task_struct * task1 = (struct task_struct *) kmalloc(PAGE_SIZE, 0, MEM_KERN); /* should be page align */
	//create_task(task1, test_process1);
	/* test map page */ 
	//map_page(0xcffc0, 0, MEM_KERN, &init_page_dir);
	/*
	 *	init_console();
	 *
	 */
	/*
	asm volatile ("movw %[IP], %%ax\n\t"
		      "movw %[CS], %%bx\n\t"
		      "jmp ."
		      ::[IP] "m" (task0->task_reg.eip), [CS] "m" (task0->task_reg.cs));
		     */

	local_irq_enable();
	switch_to_ring3(task0);
	while(1);
}

