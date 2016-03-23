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
#ifdef TEST_TASK
	task * init_task = (task *) kmalloc(PAGE_SIZE, PAGE_SIZE);
	init_task(init_task);
#endif
	init_console();
	while(1);
	return ;
}

