#include"sys_call.h"

/* we can not do like this */

 __attribute__((regparm(3))) int sys_call(u32 arg1, u32 arg2, u32 arg3)
{
	switch(arg1) {

	case _SYS_FORK:
		do_sys_fork();
		break;
	default:
		break;
	}
	return 0;
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
