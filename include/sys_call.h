#ifndef _H_SYSCALL
#define _H_SYSCALL

/* we know only five data is used */
#define MAX_SYS_CALL_NR 300

#define _SYS_OPEN	(1<<0)
#define _SYS_READ	(1<<1)
#define _SYS_WRITE	(1<<2)
#define _SYS_CLOSE	(1<<3)
#define _SYS_SEEK	(1<<4)
#define _SYS_IOCTL	(1<<5)

#define _SYS_FORK 	(1<<6)
#define _SYS_EXEC 	(1<<7)

#define SYS_CALL_5(i, func) SYS_CALL_4(u32 t5, 

#define SYS_CALL(i, func, nr_param) SYS_CALL_##nr_param(i, func)

/*
#define SYS_CALL(i, name) extern asmlinkage do_sys_##name();
*/

#endif
