#include"fork.h"
#include"schdule.h"
#include"sys_call.h"
#include"linkage.h"

asmlinkage pid_t do_sys_fork()
{
	/*
	struct task_struct *task = create_task(current, idle_task, 0);
	if (!task)
		return -1;
	return task->pid;
	*/
}

asmlinkage void do_sys_exec(char *argc)
{
	/* load certain bin file */
}


