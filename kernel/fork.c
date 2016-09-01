#include"fork.h"
#include"schdule.h"

pid_t do_fork()
{
	struct task_struct *task = (struct task_struct *)kmalloc(PAGE_SIZE, 0, MEM_KERN);
	/* copy on write */

	return pid_t;
}

void do_exec(char *argc)
{
	/* load certain bin file */
}


