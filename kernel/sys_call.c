#include"sys_call.h"

/* we can not do like this */

asmlinkage sys_call_depatch(u32 arg1, u32 arg2, u32 arg3, u32 arg4, u32 arg5)
{
	switch(arg1) {

	case _SYS_FORK:
		do_sys_fork();
		break;
	default:
		break;
	}
}	
/*
#define sys_call_ptr asmlinkage void (*_sys_call_ptr)(void)
#define SYS_CALL(i, func) [i] = func

static sys_call_ptr sys_call_table [MAX_SYS_CALL_NR] =
{
	SYS_CALL(_SYS_OPEN, do_sys_open),
	SYS_CALL(_SYS_WRITE, do_sys_write),
	SYS_CALL(_SYS_READ, do_sys_read),
	SYS_CALL(_SYS_SEEK, do_sys_seek),
	SYS_CALL(_SYS_CLOSE, do_sys_close),
	SYS_CALL(_SYS_IOCTL, do_sys_ioctl),

	SYS_CALL(_SYS_FORK, do_sys_fork),
	SYS_CALL(_SYS_EXEC, do_sys_exec),
}
*/
