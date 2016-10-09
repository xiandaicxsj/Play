#include"fork.h"
#include"schdule.h"
#include"sys_call.h"
#include"linkage.h"

asmlinkage pid_t do_sys_fork()
{
	/*struct task_struct *task = (struct task_struct *)kmalloc(PAGE_SIZE, 0, MEM_KERN);
	 */
	/* copy on write */

	return 0;
}

asmlinkage void do_sys_exec(char *argc)
{
	/* load certain bin file */
}


