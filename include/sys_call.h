#ifndef _H_SYSCALL
#define _H_SYSCALL

#include"linkage.h"
#include"schdule.h"
/* we know only five data is used */
#define MAX_SYS_CALL_NR 300

#define _SYS_OPEN	(1)
#define _SYS_READ	(2)
#define _SYS_WRITE	(3)
#define _SYS_CLOSE	(4)
#define _SYS_SEEK	(5)
#define _SYS_IOCTL	(6)

#define _SYS_FORK 	(7)
#define _SYS_EXEC 	(8)

#define P(t,s) t s
#define VAR_5 VAR_4, P(t5, s5)
#define VAR_4 VAR_3, P(t4, s4)
#define VAR_3 VAR_2, P(t3, s3)
#define VAR_2 VAR_1, P(t2, s2)
#define VAR_1 P(t1, s1)

#define SYS_CALL_DEF(func, nr_param) extern asmlinkage do_##func(VAR_##nr_param)

/*
#define SYS_CALL(i, name) extern asmlinkage do_sys_##name();
*/
extern asmlinkage pid_t do_sys_fork();
extern __attribute__((regparm(3))) int sys_call(u32 arg1, u32 arg2, u32 arg3);
#endif
